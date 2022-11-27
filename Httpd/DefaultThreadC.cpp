#include <iostream>
#include <ctime>
#include <signal.h>
#include "include/FileStream.hxx"
#include "include/HttpdThreadPool.hxx"
#include "include/HttpdPoll.hxx"
#include "include/Logger.hxx"
#include "include/HttpdSession.hxx"

#ifdef ABXHTTPD_SSL
#include "Extension/SSL.H"
#endif

namespace abxhttpd {

    void* _ThreadHandler(void* _ptr) {
        SocketRequestWithSL* SourceRequest = static_cast<SocketRequestWithSL*>(_ptr);
        HttpdSocket* SocketStream=SourceRequest->socket_p;
        bool need_free=SourceRequest->free;
        SocketRequest src = (SourceRequest->socket_p->info());
        ABXHTTPD_INFO_PRINT(4, "[Socket %d]Entered thread.", src._ad);
        std::string & _ip(src.src_in_ip);
        std::string SocketResponse;
        std::string SocketRequest;
        bool is_keep = false;
        {
            SocketRequest.clear();
            SocketResponse.clear();
            *SocketStream >> SocketRequest;
            size_t _recv_len = SocketRequest.size();
            ABXHTTPD_INFO_PRINT(105, "[Socket %d]\n[IStream]\n%s[End IStream]\n", src._ad, SocketRequest.c_str());
            HttpRequest H_req;
            HttpResponse H_res;
            if (_recv_len > 0) {
                try {
                    H_req = src.MCore->IFilter(SocketRequest, &src);
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Invoked istream filiter, handled %lu size.", src._ad, SocketRequest.size());
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Logged this request.", src._ad);
                    H_req.variables("REMOTE_ADDR", _ip);
                    H_req.variables("REMOTE_PORT", std::to_string(src.port_in));
                    if(H_req.header("Connection")=="keep-alive"){
                        H_res.header("Connection","keep-alive");
                    }else{
                        H_res.header("Connection","close");
                    }
                    if(H_req.cookie(ABXHTTPD_SESSION_STR).size()==0){
                        std::string && cook = HttpdSession::allocate();
                        H_res.set_cookie(ABXHTTPD_SESSION_STR, cook);
                        H_req.cookies()[ABXHTTPD_SESSION_STR]=cook;
                    }
                    src.MCore->Handler(H_res,H_req, &src);
                    Httpd::success_logger->write(_ip + " " +H_res.header("Content-Length")+" "+  H_req.method() + " " + H_req.path() +" " + H_req.query_string()+ " " + H_req.header("User-Agent"));
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Invoked core handler.", src._ad);
                    SocketResponse = src.MCore->OFilter(H_res, &src);
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]Invoked ostream filiter, handled %lu size.", src._ad, SocketResponse.size());
                    is_keep = (H_res.header("Connection") == "keep-alive");
                }
                catch (const HttpException & e) {
                    Httpd::except_logger->write(_ip + " [Except][" + std::to_string(e.code())+"]  " + e.what());
                    ABXHTTPD_INFO_PRINT(4, "[Socket %d]An error occured, logged this error.", src._ad);
                    HttpResponse except_page(e.html(),e.code());
                    if(e.code()==301||e.code()==302){
                        except_page.header("Location", e.what());
                    }
                    SocketResponse = except_page.raw();
                }catch (const BasicException &e){
                    Httpd::except_logger->write(_ip + " [Except][500]  " + e.what());
                    HttpException except_info(500);
                    HttpResponse except_page(except_info.html(),except_info.code());
                    SocketResponse = except_page.raw();
                }
                *SocketStream << SocketResponse;
                if (H_res.need_send_from_stream) {
                    FileStream FileToSend(H_res.need_send_from_stream_src.c_str());
                    *SocketStream << FileToSend;
                }
                ABXHTTPD_INFO_PRINT(105, "[Socket %d]\n[OStream]\n%s[End OStream]\n", src._ad, SocketResponse.c_str());
            }
        }
        if((!is_keep)||SocketStream->status()==0){
            SocketStream->close();
            if(need_free){
                delete SocketStream;
                SourceRequest->socket_p=NULL;
                delete static_cast<SocketRequestWithSL*>(_ptr);
                _ptr = NULL;
            }
        }
        return _ptr;
    }

    void* _ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args, int StreamType) {
        HttpdSettingList args = *(HttpdSettingList*)_args;
        ABXHTTPD_INFO_PRINT(2, "[Core]Entered main ThreadController, multi-thread status: %s", _set.Multithreading ? "Enabled" : "Disabled");
        HttpdThreadPool thread_pool(_set.ThreadCount, _ThreadHandler);
        while (_set.Running) {
            struct sockaddr_in src_in;
            socklen_t sklen = sizeof(src_in);
            int ad = -1;
            while (ad<=0) {
                ABXHTTPD_INFO_PRINT(11, "[Core][System API]Now invoke accept.");
                ad = accept(_set.SocketMainID, (struct sockaddr*)&src_in, &sklen);
                ABXHTTPD_INFO_PRINT(11, "[Core][System API]Invoked accept, returning %d.", ad);
            }
            ABXHTTPD_INFO_PRINT(3, "[Core]Accepted connect request from %s, allocated socket ID %d.", inet_ntoa(src_in.sin_addr), ad);
            SocketRequest _src;
            _src._ad = ad;
            _src.src_in_ip = std::string(inet_ntoa(src_in.sin_addr));
            _src._sd = _set.SocketMainID;
            _src.src_in = src_in;
            _src.port_in=htons(src_in.sin_port);
            _src.MCore = &_core;
            _src.Http_S = args.Http_S;
            HttpdSocket * SocketS;
            switch (StreamType) {
            case ABXHTTPD_SOCK_STREAM:
                SocketS = new HttpdSocket(_src);
                break;
            #ifdef ABXHTTPD_SSL
            case ABXHTTPD_SSL_STREAM:
                SocketS = new SSLSocket(_src);
                break;
            #endif
            default:
                break;
            }
            SocketRequestWithSL* __src = new SocketRequestWithSL({&_set,SocketS,true});
            if (_set.Multithreading) {
                try {
                    while (!thread_pool.push(__src)) {}
                    ABXHTTPD_INFO_PRINT(4, "[Core]Allocated thread for socket %d.", ad);
                }
                catch (const std::exception & e) {
                    ABXHTTPD_INFO_PRINT(1, "[Core]Cannot create thread for socket %d:%s.", ad, e.what());
                    Httpd::except_logger->write(std::string("[THREAD ERROR] ") + e.what());
                }
            }
            else {
                _ThreadHandler(__src);
            }
        }
        return NULL;
    }

    void* _ThreadController_POLL(const ThreadSettingList& _set, const CCore& _core, void* _args, int StreamType){
        HttpdSettingList args = *(HttpdSettingList*)_args;
        ABXHTTPD_INFO_PRINT(2, "[Core]Entered main ThreadController, multi-thread status: %s", _set.Multithreading ? "Enabled" : "Disabled");
        HttpdPoll spoll(_set,_core,&args,StreamType);
        spoll.run();
        return NULL;
    }

    void* __ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args) {
        if(_set.Multithreading&&_set.ThreadCount>0){
            _ThreadController(_set, _core, _args, ABXHTTPD_SOCK_STREAM);
        }else{
            _ThreadController_POLL(_set, _core, _args, ABXHTTPD_SOCK_STREAM);
        }
        return NULL;
    }

    ThreadController DefaultThreadC = __ThreadController;
}
