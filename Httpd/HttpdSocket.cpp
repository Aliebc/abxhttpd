#include "include/HttpdSocket.hxx"
#include <string.h>
#include <errno.h>

namespace abxhttpd{
#ifdef ABXHTTPD_UNIX
int internal_close(int ad){
    return close(ad);
}
#endif

int HttpdSocket::__close_socket(int ad)
{
    if(ad<=0){
        return 0;
    }
    int st=-1;
    ::shutdown(ad, 2);
#ifdef ABXHTTPD_WINDOWS
    st=::closesocket(ad);
    ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked closesocket, returning %d.",ad,st);
#endif
#ifdef ABXHTTPD_UNIX
    st=internal_close(ad);
    ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked close, returning %d.",ad,st);
    if(st==0){
        ABXHTTPD_INFO_PRINT(3,"[Socket %d]Closed.",ad);
    }else{
        ABXHTTPD_INFO_PRINT(3,"Cannot close socket %d(Error code:%d)",ad,st);
    }
#endif
    return st;
}

bool HttpdSocket::__set_socket_timeout(int ad,int second){
    int st1=-1,st2=-1;
#ifdef ABXHTTPD_UNIX
    struct timeval timout{second,0};
#else
    int timout=second*1000;
#endif
    st1=setsockopt(ad, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&timout), sizeof(timout));
    st2=setsockopt(ad, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&timout), sizeof(timout));
    if((st1&st2)==0){
        return true;
    }
    return false;
}

HttpdSocket::HttpdSocket(const SocketRequest & _sk_req):HttpdSocket()
{
    __set_socket_timeout(_sk_req._ad, ABXHTTPD_SOCKET_TIMEOUT);
    _src=_sk_req;
}
HttpdSocket::HttpdSocket():_src{}
{
    status_id|=(READABLE|FLAG::WRITEABLE);
}
const SocketRequest & HttpdSocket::info() const
{
    return _src;
}
size_t HttpdSocket::read(std::string & _dst,size_t size)
{
    clear_tmp();
    size_t _recv_len=0;
    if(size==0){--size;}
    while(true){
        long _recv_s=0;
        int ad=_src._ad;
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke recv.",ad);
        _recv_s=recv(ad,buffer_tmp,ABXHTTPD_MIN(buffer_size(),size),0);
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked recv, returning %ld.",ad,_recv_s);
        if(_recv_s>0){
            _recv_len+=_recv_s;
            _dst.insert(_dst.size(),buffer_tmp,_recv_s);
            ABXHTTPD_INFO_PRINT(3,"[Socket %d]Received %ld bytes.",ad,_recv_s);
            clear_tmp();
            size-=_recv_s;
            if(_recv_s<buffer_size()){
                break;
            }
        }else{
            SetLastError(strerror(errno));
            this->close();
            return 0;
        }
    }
    return _recv_len;
}
size_t HttpdSocket::write(const std::string & _res,size_t size)
{
    clear_tmp();
    size_t _send_len=0;
    if(size==0){--size;}
    size_t _r_send_len=ABXHTTPD_MIN(_res.size(),size);
    while(_send_len<_r_send_len){
        int ad=_src._ad;
        long int _send_lv=-1;
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke send.",ad);
        _send_lv=send(ad,_res.c_str()+_send_len,_r_send_len-_send_len,0);
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked send, returning %ld.",ad,_send_lv);
        ABXHTTPD_INFO_PRINT(105,"[Socket %d]\n[OStream]\n%s[End OStream]\n",ad,_res.c_str());
        if(_send_lv>0){
            _send_len+=_send_lv;
        }else{
            SetLastError(strerror(errno));
            this->close();
            break;
        }
    }
    return _send_len;
}
HttpdSocket::~HttpdSocket()
{
    if(status_id!=0){
        this->close();
    }
}
bool HttpdSocket::close()
{
    if(status_id!=0){
        status_id=0;
        return (__close_socket(_src._ad)==0);
    }
    return true;
}
}
