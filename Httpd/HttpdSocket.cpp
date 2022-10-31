#include "include/abxhttpd.H"
#include <string.h>
#include <errno.h>

namespace abxhttpd{

    int HttpdSocket::__close_socket(int ad)
    {
        if(ad<=0){
            return 0;
        }
        int st;
        ::shutdown(ad, 2);
        #ifdef ABXHTTPD_WINDOWS
        st=::closesocket(ad);
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked closesocket, returning %d.",ad,st);
        #endif
        #ifdef ABXHTTPD_UNIX
        st=::close(ad);
        ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked close, returning %d.",ad,st);
        if(st==0){
            ABXHTTPD_INFO_PRINT(3,"[Socket %d]Closed.",ad);
        }else{
            ABXHTTPD_INFO_PRINT(3,"Cannot close socket %d(Error code:%d)",ad,st);
        }
        #endif
        return st;
    }

    HttpdSocket::HttpdSocket(const SocketRequest & _sk_req):HttpdSocket()
    {
        _src=_sk_req;
    }
    HttpdSocket::HttpdSocket()
    {
        st=1;
        last_err="success";
    }
    const SocketRequest & HttpdSocket::info()
    {
        return _src;
    }
    size_t HttpdSocket::read(std::string & _dst,size_t size)
    {
        memset(tmp,0,sizeof(tmp));
        int ad=_src._ad;
        long int _recv_s=0;
        size_t _recv_len=0;
        if(size==0){
            size-=1;
        }
        if(size){
            while(size>0){
                ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke recv.",ad);
                _recv_s=recv(ad,tmp,ABXHTTPD_MIN(sizeof(tmp),size),0);
                ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked recv, returning %ld.",ad,_recv_s);
                if(_recv_s>0){
                    _recv_len+=_recv_s;
                    _dst.insert(_dst.size(),tmp,_recv_s);
                    ABXHTTPD_INFO_PRINT(3,"[Socket %d]Received %ld bytes.",ad,_recv_s);
                    memset(tmp,0,sizeof(tmp));
                    size-=_recv_s;
                    if(_src.is_noblocked){
                        break;
                    }
                }else if(_recv_s==0){
                    last_err=strerror(errno);
                    this->close();
                    return 0;
                }else if(_recv_s<0){
                    last_err=strerror(errno);
                    ABXHTTPD_INFO_PRINT(11, "[Socket %d][System API]Error:%s.", ad, last_err);
                    if(errno==EWOULDBLOCK||errno==EAGAIN){
                        continue;
                    }else{
                        this->close();
                        return 0;
                    }
                }
            }
        }
        return _recv_len;
    }
    size_t HttpdSocket::write(const std::string & _res,size_t size)
    {
        memset(tmp,0,sizeof(tmp));
        int ad=_src._ad;
        size_t _send_len=0;
        long int _send_lv=-1;
        if(size==0){
            size-=1;
        }
        size_t _r_send_len=ABXHTTPD_MIN(_res.size(),size);
        while(_send_len<_r_send_len){
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke send.",ad);
            _send_lv=send(ad,_res.c_str()+_send_len,_r_send_len-_send_len,MSG_DONTWAIT);
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked send, returning %ld.",ad,_send_lv);
            ABXHTTPD_INFO_PRINT(105,"[Socket %d]\n[OStream]\n%s[End OStream]\n",ad,_res.c_str());
            if(_send_lv>0){
                _send_len+=_send_lv;
            }else if(_send_lv<=0){
                last_err=strerror(errno);
                if(_src.is_noblocked){
                    if(errno==0||errno==EWOULDBLOCK){
                        continue;
                    }else{
                        this->close();
                        break;
                    }
                }else{
                    this->close();
                    break;
                }
            }
        }
        return _send_len;
    }
    HttpdSocket::~HttpdSocket()
    {
        if(st!=0){
            close();
        }
    }
    int HttpdSocket::status()
    {
        return st;
    }
    bool HttpdSocket::close()
    {
        if(st!=0){
            st=0;
            return (__close_socket(_src._ad)==0);
        }
        return true;
    }
    const char * HttpdSocket::GetLastError()
    {
        return last_err;
    }
    HttpdSocket & operator>> ( HttpdSocket & src, std::string & is){
        src.read(is);
        return src;
    }
    HttpdSocket & operator<< (HttpdSocket & src, std::string & in){
        src.write(in);
        return src;
    }
}
