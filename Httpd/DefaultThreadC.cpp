#include "include/abxhttpd.H"
#include <iostream>
#include <ctime>
#include <signal.h>
#include "include/FileSocket.hxx"
#include "include/HttpdThreadPool.hxx"

#ifdef ABXHTTPD_SSL
#include "Extension/SSL.H"
#endif

namespace abxhttpd {
    std::mutex io_lock;
    typedef struct _SocketRequestWithSL
    {
        SocketRequest socket_r;
        ThreadSettingList thread_s;
        int StreamID;
    }SocketRequestWithSL;

    void* _ThreadHandler(void* _ptr);

    void* _ThreadHandler(void* _ptr) {
        SocketRequestWithSL* SourceRequest = (SocketRequestWithSL*)_ptr;
        char _time[128];
        time_t tt;
        SocketRequest src = (SourceRequest->socket_r);
        std::ostream* logout = SourceRequest->thread_s.abxout;
        std::ostream* errout = SourceRequest->thread_s.abxerr;
        ABXHTTPD_INFO_PRINT(4, "[Socket %d]Entered thread.", src._ad);
        std::string& _ip = SourceRequest->socket_r.src_in_ip;
        std::string SocketResponse;
        std::string SocketRequest;
        int SocketStreamType = SourceRequest->StreamID;
        std::unique_ptr<HttpdSocket> SocketStream;
        switch (SocketStreamType) {
        case ABXHTTPD_SOCK_STREAM:
            SocketStream = std::move(std::unique_ptr<HttpdSocket>(new HttpdSocket(src)));
            break;
#ifdef ABXHTTPD_SSL
        case ABXHTTPD_SSL_STREAM:
            SocketStream = std::move(std::unique_ptr<HttpdSocket>(new SSLSocket(src)));
            break;
#endif
        default:
            break;
        }
        bool is_keep = false;
        do {
            SocketRequest.clear();
            SocketRequest.shrink_to_fit();
            SocketResponse.clear();
            SocketResponse.shrink_to_fit();
            *SocketStream >> SocketRequest;
            tt = time(NULL);
            strftime(_time, 128, "[%Y-%m-%d %H:%M:%S] ", localtime(&tt));
            if (SocketStream->status() == 0) {
                delete (SocketRequestWithSL*)_ptr;
                return NULL;
            }
            size_t _recv_len = SocketRequest.size();
            ABXHTTPD_INFO_PRINT(105, "[Socket %d]\n[IStream]\n%s[End IStream]\n", src._ad, SocketRequest.c_str());
            HttpRequest H_req;
            HttpResponse H_res;
            if (_recv_len > 0) {
                try {
                    H_req = src.MCore.IFilter(SocketRequest, _ptr);
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Invoked istream filiter, handled %lu size.", src._ad, SocketRequest.size());
                    H_req.remote_addr() = _ip;
                    {
                        std::unique_lock<std::mutex> iock(io_lock);
                        *logout << _time << _ip << " " << H_req.method() << " " << H_req.path() << " " << H_req.header("User-Agent") << std::endl;
                    }
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Logged this request.", src._ad);
                    H_res = src.MCore.Handler(H_req, _ptr);
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Invoked core handler.", src._ad);
                    SocketResponse = src.MCore.OFilter(H_res, _ptr);
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Invoked ostream filiter, handled %lu size.", src._ad, SocketResponse.size());
                    is_keep = (H_res.header("Connection") == "keep-alive");
                }
                catch (abxhttpd_error& e) {
                    *errout << _time << _ip << " Error:" << e.what() << std::endl;
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]An error occured, logged this error.", src._ad);
                    char bd[] = "HTTP/1.1 400 Bad Request";
                    SocketResponse = bd;
                }
                *SocketStream << SocketResponse;
                if (H_res.need_send_from_stream) {
                    FileSocket FileToSend(H_res.need_send_from_stream_src.c_str());
                    *SocketStream << FileToSend;
                }
                ABXHTTPD_INFO_PRINT(105, "[Socket %d]\n[OStream]\n%s[End OStream]\n", src._ad, SocketResponse.c_str());
            }
        } while (is_keep);
        SocketStream->close();
        delete (SocketRequestWithSL*)_ptr;
        return NULL;
    }

    void* _ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args, int StreamType) {
        HttpdSettingList args = *(HttpdSettingList*)_args;
        ABXHTTPD_INFO_PRINT(2, "[Core]Entered main ThreadController, multi-thread status: %s", _set.Multi_thread ? "Enabled" : "Disabled");
        HttpdThreadPool thread_pool(_set.Thread_count, _ThreadHandler);
        while (_set.Is_running) {
            struct sockaddr_in src_in;
            socklen_t sklen = sizeof(src_in);
            int ad = -1;
#ifdef ABXHTTPD_UNIX
            signal(SIGPIPE, SIG_IGN);
            ABXHTTPD_INFO_PRINT(11, "[Core][System API]Now invoke accept.");
            ad = accept(_set.Socket_n, (struct sockaddr*)&src_in, &sklen);
            ABXHTTPD_INFO_PRINT(11, "[Core][System API]Invoked accept, returning %d.", ad);
#endif
#ifdef ABXHTTPD_WINDOWS
            while (ad <= 0) {
                if (!_set.Is_running) {
                    break;
                }
                ABXHTTPD_INFO_PRINT(11, "[Core][System API]Now invoke accept.");
                ad = accept(_set.Socket_n, (struct sockaddr*)&src_in, &sklen);
                ABXHTTPD_INFO_PRINT(11, "[Core][System API]Invoked accept, returning %d.", ad);
            }
#endif
            if (ad < 0) {
                continue;
            }
            ABXHTTPD_INFO_PRINT(3, "[Core]Accepted connect request from %s, allocated socket ID %d.", inet_ntoa(src_in.sin_addr), ad);
            SocketRequest _src;
            _src._ad = ad;
            _src.is_noblocked = true;
            _src.src_in_ip = std::string(inet_ntoa(src_in.sin_addr));
            _src._sd = _set.Socket_n;
            _src.src_in = src_in;
            _src.MCore = _core;
            _src.Http_S = args.Http_S;
            SocketRequestWithSL* __src = new SocketRequestWithSL({ _src,_set });
            __src->StreamID = StreamType;
            if (_set.Multi_thread) {
                try {
                    while (!thread_pool.push(__src)) {}
                    ABXHTTPD_INFO_PRINT(4, "[Core]Allocated thread for socket %d.", ad);
                }
                catch (std::exception e) {
                    ABXHTTPD_INFO_PRINT(1, "[Core]Cannot create thread for socket %d:%s.", ad, e.what());
                    *_set.abxerr << "THREAD ERROR:" << e.what() << std::endl;
                }
            }
            else {
                _ThreadHandler(__src);
            }
        }
        return NULL;
    }

    void* __ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args) {
        _ThreadController(_set, _core, _args, ABXHTTPD_SOCK_STREAM);
        return NULL;
    }

    ThreadController DefaultThreadC = __ThreadController;
}
