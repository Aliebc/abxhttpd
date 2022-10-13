#include "include/abxhttpd.H"
#include <pthread.h>
#include <unistd.h>
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
            while(true){
                struct sockaddr_in src_in;
                socklen_t sklen=sizeof(src_in);
                int ad=-1;
                #ifdef ABXHTTPD_UNIX
                signal(SIGPIPE, SIG_IGN);
                ad=accept(_set.Socket_n,(struct sockaddr *)&src_in,&sklen);
                #endif
                #ifdef ABXHTTPD_WINDOWS
                while(ad<=0){
                    ad=accept(_set.Socket_n,(struct sockaddr *)&src_in,&sklen);
                    usleep(1000L*100);
                }
                #endif
                if(ad<0){
                    #ifdef ABXHTTPD_DEBUG
                    printf("Cannot accept,continue....\n");
                    #endif
                    continue;
                }
                #ifdef ABXHTTPD_DEBUG
                printf("Accepted socket %d....\n",ad);
                #endif
                SocketRequest _src;
                _src._ad=ad;
                _src._sd=_set.Socket_n;
                _src.src_in=src_in;
                _src.MCore=_core;
                _src.Http_S=args.Http_S;
                SocketRequestWithSL __src={_src,_set};
                if(_set.Multi_thread){
                    try{
                        std::thread _th(_ThreadHandler,&__src);
                        _th.detach();
                    }catch(std::exception e){
                        *_set.abxerr << "THREAD ERROR:" << e.what() <<std::endl;
                    }
                }else{
                    _ThreadHandler(&__src);
                }
            }
        return NULL;
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
        ssize_t _recv_s=0;
        ssize_t _recv_len=0;
        time_t _ReadBegin=time(NULL);
        time_t _ReadEnd=time(NULL);
        int _retry=0;
        std::string _ip(inet_ntoa(src.src_in.sin_addr));
        std::string res;
        std::string req;
        while(true){
            _recv_s=recv(ad,_req,sizeof(_req),MSG_DONTWAIT);
            if(_recv_s>0){
                _ReadBegin=time(NULL);
                #ifdef ABXHTTPD_DEBUG
                printf("[Socket %d]Receive %ld size.\n",ad,_recv_s);
                #endif
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
                        break;
                    }
                    continue;
                }else{
                    break;
                }
            }else if(_recv_s==0){
                #ifdef ABXHTTPD_DEBUG
                printf("[Socket %d]Closed by peer.\n",ad);
                #endif
                break;
            }
            //usleep(1000L*100);
        }
        if(_recv_len>0){
            try{
                HttpRequest H_req=src.MCore.IFilter(req,_ptr);
                H_req.remote_addr()=_ip;
                *logout<< _time << _ip << " " << H_req.method() <<" "<< H_req.path() << " "<< H_req.header("User-Agent") <<std::endl;
                HttpResponse H_res=src.MCore.Handler(H_req,_ptr);
                res=src.MCore.OFilter(H_res,_ptr);
            }catch(abxhttpd_error e){
                *errout<< _time << inet_ntoa(src.src_in.sin_addr) << " Error:" << e.what()<<std::endl;
                char bd[]="HTTP/1.1 400 Bad Request";
                res=std::string(bd);
            }
            long int _send_lv=-1;
            size_t _send_len=0;
            while(true){
                _send_lv=send(ad,res.c_str()+_send_len,res.size()-_send_len,MSG_DONTWAIT);
                if(_send_lv>0){
                    _send_len+=_send_lv;
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
            
            #ifdef ABXHTTPD_WINDOWS
            shutdown(ad,2);
            #endif
            #ifdef ABXHTTPD_UNIX
            int st;
            st=close(ad);
            if(st==0){
                #ifdef ABXHTTPD_DEBUG
                printf("Closed Socket %d.\n",ad);
                #endif
            }else{
                #ifdef ABXHTTPD_DEBUG
                printf("Cannot close socket %d(Error code:%d)",ad,st);
                #endif
            }
            #endif
        }else{
            int st;
            #ifdef ABXHTTPD_WINDOWS
            st=shutdown(ad,2);
            #endif
            #ifdef ABXHTTPD_UNIX
            st=close(ad);
            if(st==0){
                #ifdef ABXHTTPD_DEBUG
                printf("Closed Socket %d.\n",ad);
                #endif
            }else{
                #ifdef ABXHTTPD_DEBUG
                printf("Cannot close socket %d(Error code:%d)",ad,st);
                #endif
            }
            #endif
        }
        return _ptr;
    }

    ThreadController DefaultThreadC=_ThreadController;
}