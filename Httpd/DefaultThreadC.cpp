#include "include/abxhttpd.H"
#include <iostream>
#include <thread>
#include <ctime>
#include <signal.h>

#ifdef ABXHTTPD_WINDOWS

#define MSG_DONTWAIT 0x0

#endif


namespace abxhttpd{
    typedef struct _SocketRequestWithSL
    {
        SocketRequest sr;
        ThreadSettingList ts;
    }SocketRequestWithSL;

    void * _ThreadHandler(void * _ptr);

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
                _src._sd=_set.Socket_n;
                _src.src_in=src_in;
                _src.MCore=_core;
                _src.Http_S=args.Http_S;
                SocketRequestWithSL * __src=new SocketRequestWithSL({_src,_set});
                if(_set.Multi_thread){
                    try{
                        std::thread _th(_ThreadHandler,__src);
                        _th.detach();
                        ABXHTTPD_INFO_PRINT(4,"[Core]Detached thread for socket %d.",ad);
                    }catch(std::exception e){
                        ABXHTTPD_INFO_PRINT(1,"[Core]Cannot create thread for socket %d:%s.",ad,e.what());
                        *_set.abxerr << "THREAD ERROR:" << e.what() <<std::endl;
                    }
                }else{
                    _ThreadHandler(__src);
                }
            }
        return NULL;
    }

    void close_socket(int ad){
        int st;
        #ifdef ABXHTTPD_WINDOWS
        st=shutdown(ad,2);
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked shutdown, returning %d.",ad,st);
        #endif
        #ifdef ABXHTTPD_UNIX
        st=close(ad);
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked close, returning %d.",ad,st);
        if(st==0){
            ABXHTTPD_INFO_PRINT(3,"[Socket %d]Closed.",ad);
        }else{
            ABXHTTPD_INFO_PRINT(3,"Cannot close socket %d(Error code:%d)",ad,st);
        }
        #endif
    }

    void * _ThreadHandler(void * _ptr){
        char _req[1024];
        char _time[128];
        time_t tt=time(NULL);
        strftime(_time,128,"[%Y-%m-%d %H:%M:%S] ",localtime(&tt));
        SocketRequestWithSL src_sl=*(SocketRequestWithSL *)_ptr;
        SocketRequest src =(src_sl.sr);
        std::ostream *logout=src_sl.ts.abxout;
        std::ostream *errout=src_sl.ts.abxerr;
        int ad=src._ad;
        ABXHTTPD_INFO_PRINT(4,"[Socket %d]Entered thread.",ad);
        long int _recv_s=0;
        long int _recv_len=0;
        time_t _ReadBegin=time(NULL);
        time_t _ReadEnd=time(NULL);
        int _retry=0;
        std::string _ip(inet_ntoa(src.src_in.sin_addr));
        std::string res;
        std::string req;
RE_RECV:
        req.clear();
        res.clear();
        while(true){
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke recv.",ad);
            _recv_s=recv(ad,_req,sizeof(_req),0);
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked recv, returning %ld.",ad,_recv_s);
            if(_recv_s>0){
                _ReadBegin=time(NULL);
                ABXHTTPD_INFO_PRINT(3,"[Socket %d]Received %ld bytes.",ad,_recv_s);
                _recv_len+=_recv_s;
                req+=std::string(_req,_recv_s);
                memset(_req,0,sizeof(_req));
                if(_recv_s==sizeof(_req)){
                    continue;
                }else{
                    break;
                }
            }else if(_recv_s<0){
                if(errno==EWOULDBLOCK||errno==EAGAIN){
                    _ReadEnd=time(NULL);
                    if((_ReadEnd-_ReadBegin)>3){
                        *errout << "Read TIMEOUT:" <<strerror(errno) <<std::endl;
                        ABXHTTPD_INFO_PRINT(1,"[Socket %d]Read timeout.",ad);
                        break;
                    }
                    continue;
                }else{
                    break;
                }
            }else if(_recv_s==0){
                ABXHTTPD_INFO_PRINT(3,"[Socket %d]Closed by remote.",ad);
                close_socket(ad);
                return NULL;
            }
            //usleep(1000L*100);
        }
        bool is_keep=false;
        if(_recv_len>0){
            try{
                HttpRequest H_req;
                HttpResponse H_res;
                H_req=src.MCore.IFilter(req,_ptr);
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Invoked istream filiter, handled %ld size.",ad,req.size());
                H_req.remote_addr()=_ip;
                *logout<< _time << _ip << " " << H_req.method() <<" "<< H_req.path() << " "<< H_req.header("User-Agent") <<std::endl;
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Logged this request.",ad);
                H_res=src.MCore.Handler(H_req,_ptr);
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Invoked core handler.",ad);
                res=src.MCore.OFilter(H_res,_ptr);
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]Invoked ostream filiter, handled %ld size.",ad,res.size());
                is_keep=(H_res.header("Connection")=="keep-alive");
            }catch(abxhttpd_error e){
                *errout<< _time << inet_ntoa(src.src_in.sin_addr) << " Error:" << e.what()<<std::endl;
                ABXHTTPD_INFO_PRINT(4,"[Socket %d]An error occured, logged this error.",ad);
                char bd[]="HTTP/1.1 400 Bad Request";
                res=std::string(bd);
            }
            long int _send_lv=-1;
            size_t _send_len=0;
            while(true){
                ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke send.",ad);
                _send_lv=send(ad,res.c_str()+_send_len,res.size()-_send_len,MSG_DONTWAIT);
                ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked send, returning %ld.",ad,_send_lv);
                if(_send_lv>0){
                    _send_len+=_send_lv;
                    ABXHTTPD_INFO_PRINT(3,"[Socket %d]Wrote %ld bytes.",ad,_send_len);
                    if(_send_len==res.size()){
                        break;
                    }
                    
                }else if(_send_lv<0){
                    if(errno==0||errno==EWOULDBLOCK){
                        continue;
                    }else{
                        break;
                    }
                }else if(_send_lv==0){
                    //PP..
                    continue;
                }
            }
            if(is_keep){
                goto RE_RECV;
            }
            close_socket(ad);
        }else{
            close_socket(ad);
        }
        delete (SocketRequestWithSL *)_ptr;
        return NULL;
    }

    ThreadController DefaultThreadC=_ThreadController;
}