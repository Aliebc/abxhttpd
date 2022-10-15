#ifndef HTTPDSOCKET_H
#define HTTPDSOCKET_H

#include "abxhttpd.H"
#ifdef ABXHTTPD_WINDOWS

#define MSG_DONTWAIT 0x0

#endif

#define ABXHTTPD_MIN(a,b) (a<b?a:b)

namespace abxhttpd{
    class HttpdSocket
    {
    public:
        HttpdSocket();
        HttpdSocket(SocketRequest &);
        HttpdSocket(int); //不推荐
        bool accept();
        bool close();
        const SocketRequest & info();
        size_t read(std::string & _dst,size_t size = 0 );
        size_t write(std::string & _content,size_t size = 0);
        bool should_close();
        int status();
        int & handle_count();
        const char * GetLastError();
        ~HttpdSocket();
        template <class _A>
        friend _A & operator<< (_A & src, std::string & in){
            src.write(in);
            return src;
        }
        template <class _A>
        friend _A & operator>> ( _A & src, std::string & is){
            src.read(is);
            return src;
        }
    private:
        SocketRequest _src;
        int st;
        int _hc;
        char tmp[2048];
        char * last_err;
    };
}

#endif