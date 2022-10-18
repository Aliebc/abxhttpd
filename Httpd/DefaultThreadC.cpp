#include "include/abxhttpd.H"
#include <iostream>
#include <thread>
#include <ctime>
#include <mutex>
#include <signal.h>
#include "include/FileSocket.hxx"

#ifdef ABXHTTPD_SSL
#include "Extension/SSL.H"
#endif

namespace abxhttpd{
    std::mutex io_lock;
    typedef struct _SocketRequestWithSL
    {
        SocketRequest socket_r;
        ThreadSettingList thread_s;
        HttpdSocket * socket_s;
    }SocketRequestWithSL;

    template <class SocketStream>
    void * _ThreadHandler(void * _ptr){
        char _time[128];
        time_t tt;
        SocketRequestWithSL * src_sl=(SocketRequestWithSL *)_ptr;
        SocketRequest src =(src_sl->socket_r);
        std::ostream *logout=src_sl->thread_s.abxout;
        std::ostream *errout=src_sl->thread_s.abxerr;
        ABXHTTPD_INFO_PRINT(4,"[Socket %d]Entered thread.",src._ad);
        std::string & _ip =src_sl->socket_r.src_in_ip;
        std::string res;
        std::string req;
        SocketStream sk_stream(src);
RE_RECV:
        req.clear();
        req.shrink_to_fit();
        res.clear();
        res.shrink_to_fit();
        sk_stream >> req;
        tt=time(NULL);
        strftime(_time,128,"[%Y-%m-%d %H:%M:%S] ",localtime(&tt));
        if(sk_stream.status()==0){
            delete (SocketRequestWithSL *)_ptr;
            return NULL;
        }
        size_t _recv_len=req.size();
        ABXHTTPD_INFO_PRINT(105,"[Socket %d]\n[IStream]\n%s[End IStream]\n",src._ad,req.c_str());
        bool is_keep=false;
        HttpRequest H_req;
        HttpResponse H_res;
        if(_recv_len>0){
            try{
                H_req=src.MCore.IFilter(req,_ptr);
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Invoked istream filiter, handled %lu size.",src._ad,req.size());
                H_req.remote_addr()=_ip;
                io_lock.lock();
                *logout<< _time << _ip << " " << H_req.method() <<" "<< H_req.path() << " "<< H_req.header("User-Agent") <<std::endl;
                io_lock.unlock();
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Logged this request.",src._ad);
                H_res=src.MCore.Handler(H_req,_ptr);
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Invoked core handler.",src._ad);
                res=src.MCore.OFilter(H_res,_ptr);
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Invoked ostream filiter, handled %lu size.",src._ad,res.size());
                is_keep=(H_res.header("Connection")=="keep-alive");
            }catch(abxhttpd_error &e){
                *errout<< _time << _ip << " Error:" << e.what()<<std::endl;
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]An error occured, logged this error.",src._ad);
                char bd[]="HTTP/1.1 400 Bad Request";
                res=std::string(bd);
            }
            sk_stream << res;
            if(H_res.need_send_from_stream){
                FileSocket fio(H_res.need_send_from_stream_src.c_str());
                sk_stream << fio;
            }
            ABXHTTPD_INFO_PRINT(105,"[Socket %d]\n[OStream]\n%s[End OStream]\n",src._ad,res.c_str());
            if(is_keep){
                goto RE_RECV;
            }
            sk_stream.close();
        }else{
            sk_stream.close();
        }
        delete (SocketRequestWithSL *)_ptr;
        return NULL;
    }

    template <class SocketStream>
    void * _ThreadController (const ThreadSettingList & _set, const CCore & _core, void * _args){
        HttpdSettingList args=*(HttpdSettingList *)_args;
        ABXHTTPD_INFO_PRINT(2,"[Core]Entered main ThreadController, multi-thread status: %s",_set.Multi_thread?"Enabled":"Disabled");
            while(_set.Is_running){
                struct sockaddr_in src_in;
                socklen_t sklen=sizeof(src_in);
                int ad=-1;
                #ifdef ABXHTTPD_UNIX
                signal(SIGPIPE, SIG_IGN);
                ABXHTTPD_INFO_PRINT(11,"[Core][System API]Now invoke accept.");
                ad=accept(_set.Socket_n,(struct sockaddr *)&src_in,&sklen);
                ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked accept, returning %d.",ad);
                #endif
                #ifdef ABXHTTPD_WINDOWS
                while(ad<=0){
                    if(!_set.Is_running){
                        break;
                    }
                    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Now invoke accept.");
                    ad=accept(_set.Socket_n,(struct sockaddr *)&src_in,&sklen);
                    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked accept, returning %d.",ad);
                }
                #endif
                if(ad<0){
                    continue;
                }
                ABXHTTPD_INFO_PRINT(3,"[Core]Accepted connect request from %s, allocated socket ID %d.",inet_ntoa(src_in.sin_addr),ad);
                SocketRequest _src;
                _src._ad=ad;
                _src.is_noblocked=true;
                _src.src_in_ip=std::string(inet_ntoa(src_in.sin_addr));
                _src._sd=_set.Socket_n;
                _src.src_in=src_in;
                _src.MCore=_core;
                _src.Http_S=args.Http_S;
                SocketRequestWithSL * __src=new SocketRequestWithSL({_src,_set});
                if(_set.Multi_thread){
                    try{
                        std::thread _th(_ThreadHandler<SocketStream>,__src);
                        _th.detach();
                        ABXHTTPD_INFO_PRINT(4,"[Core]Detached thread for socket %d.",ad);
                    }catch(std::exception e){
                        ABXHTTPD_INFO_PRINT(1,"[Core]Cannot create thread for socket %d:%s.",ad,e.what());
                        *_set.abxerr << "THREAD ERROR:" << e.what() <<std::endl;
                    }
                }else{
                    _ThreadHandler<SocketStream>(__src);
                }
            }
        return NULL;
    }

    void * __ThreadController (const ThreadSettingList & _set, const CCore & _core, void * _args){
        _ThreadController<HttpdSocket>(_set,_core,_args);
        return NULL;
    }

    ThreadController DefaultThreadC=__ThreadController;
}
