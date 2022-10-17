#include "include/abxhttpd.H"

namespace abxhttpd{

    int __close_socket(int ad){
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
        return st;
    }

    HttpdSocket::HttpdSocket(SocketRequest & _sk_req)
    {
        st=1;
        _src=_sk_req;
    }
    HttpdSocket::HttpdSocket(int ad)
    {
        st=1;
        _src._ad=ad;
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
                    ABXHTTPD_INFO_PRINT(11, "[Socket %d][System API]Error:%s.", last_err);
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
    size_t HttpdSocket::write(std::string & _res,size_t size)
    {
        memset(tmp,0,sizeof(tmp));
        int ad=_src._ad;
        size_t _send_len=0;
        long int _send_lv=-1;
        if(size==0){
            size-=1;
        }
        while(_send_len<ABXHTTPD_MIN(_res.size(),size)){
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Now invoke send.",ad);
            _send_lv=send(ad,_res.c_str()+_send_len,_res.size()-_send_len,MSG_DONTWAIT);
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
                        break;
                        this->close();
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
    }
    int HttpdSocket::status()
    {
        return st;
    }
    bool HttpdSocket::close()
    {
        st=0;
        return (__close_socket(_src._ad)==0);
    }
    /*template <class _A>
    _A & operator>> ( _A & src, std::string & is){
        src.read(is);
        return src;
    }*/
}