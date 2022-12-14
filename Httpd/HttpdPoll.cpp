#include "include/HttpdPoll.hxx"
#include "Extension/SSL.H"

#define ABXHTTPD_POLL_INIT 8

namespace abxhttpd{
void* _ThreadHandler(void* _ptr);
void* _ThreadHandler2(void* _ptr);
HttpdPoll::HttpdPoll(const ThreadSettingList& _set, const CCore& _core,const HttpdSettingList * http_s,int StreamType):npfd(0){
    MSocketID=_set.SocketMainID;
    thread_s=&_set;
    this->http_s=*http_s;
    running=true;
    this->_core=&_core;
    last_err="success";
    type=StreamType;
    SocketList=(struct pollfd *)::malloc(ABXHTTPD_POLL_INIT*sizeof(struct pollfd));
    SLsize=ABXHTTPD_POLL_INIT;
    if(SocketList==NULL){
        throw BasicException("Cannot assign memory for pollfd");
    }
    SocketList[0].fd=MSocketID;
    SocketList[0].events=POLLIN;
}

bool HttpdPoll::accept(){
    int st=-1;
    struct sockaddr_in src_in;
    socklen_t sklen = sizeof(src_in);
    st=::accept(MSocketID,(struct sockaddr*)&src_in, &sklen);
    ABXHTTPD_INFO_PRINT(3,"Accpeted, allocate socket ID %d",st);
    if(st<=0){
        return false;
    }
    SocketRequest _src;
    _src._ad = st;
    _src.src_in_ip = inet_ntoa(src_in.sin_addr);
    _src.port_in=htons(src_in.sin_port);
    _src._sd = MSocketID;
    _src.MCore = _core;
    _src.Http_S=http_s.Http_S;
    HttpdSocket * SocketStream;
    switch (type) {
    case ABXHTTPD_SOCK_STREAM:
        SocketStream = new HttpdSocket(_src);
        break;
    #ifdef ABXHTTPD_SSL
    case ABXHTTPD_SSL_STREAM:
        SocketStream = new SSLSocket(_src);
        break;
    #endif
    default:
        break;
    }
    SocketRequestWithSL* __src = new SocketRequestWithSL({thread_s,SocketStream,false});
    __src->filter=new BasicHttpFilter(*SocketStream,*SocketStream);
    __src->filter->set(_core->Handler,_src);
    if(SocketMap.find(st)!=SocketMap.end()){
        delete SocketMap[st];
    }
    SocketMap[st]=__src;
    npfd=(int)SocketMap.size();
    return true;
}

bool HttpdPoll::alloc_poll(){
    npfd=(int)SocketMap.size();
    if(SLsize<=npfd){
        SocketList=(struct pollfd *)::realloc(static_cast<void*>(SocketList), 2*npfd*sizeof(struct pollfd));
        SLsize=2*npfd;
        if(SocketList==NULL){
            last_err=strerror(errno);
            return false;
        }
    }
    SocketList[0].revents=0;
    ::memset(SocketList+1,0,(SLsize-1)*sizeof(struct pollfd));
    int _p=1;
    auto _j=SocketMap.begin();
    while(_p<=npfd){
        SocketList[_p].fd=_j->first;
        SocketList[_p].events=POLLIN;
        if(_j->second->filter->status()&
           (BasicFilter::S_FLAG::CONTINUE_WRITE|BasicFilter::S_FLAG::FINISHED_WRITE)){
            SocketList[_p].events=POLLOUT;
        }
        _p++;
        _j++;
    }
    return true;
}

bool HttpdPoll::run(){
    while(running){
        alloc_poll();
        poll(SocketList,npfd+1,-1);
        if(SocketList[0].revents&POLLIN){
            this->accept();
        }
        for(int _i=1;_i<npfd+1;_i++){
            if(SocketList[_i].revents&(POLLIN|POLLOUT)){
                _ThreadHandler2(SocketMap[SocketList[_i].fd]);
                if(SocketMap[SocketList[_i].fd]->socket_p->status()==0){
                    delete SocketMap[SocketList[_i].fd];
                    SocketMap.erase(SocketList[_i].fd);
                }
            }else if(SocketList[_i].revents&(POLLNVAL|POLLHUP)){
                delete SocketMap[SocketList[_i].fd];
                SocketMap.erase(SocketList[_i].fd);
            }
        }
    }
    return true;
}

bool HttpdPoll::shutdown(){
    running=false;
    return true;
}

const char * HttpdPoll::GetLastError() noexcept{
    return last_err;
}

HttpdPoll::~HttpdPoll(){
    free(SocketList);
}
}
