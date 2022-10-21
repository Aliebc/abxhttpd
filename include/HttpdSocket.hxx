#ifndef HTTPDSOCKET_H
#define HTTPDSOCKET_H

#include "abxhttpd.H"
#include "Httpd.hxx"
#ifdef ABXHTTPD_WINDOWS

#define MSG_DONTWAIT 0x0

#endif

#define ABXHTTPD_SOCK_STREAM 0

namespace abxhttpd{
    int __close_socket(int ad);
    class HttpdSocket
    {
    public:
        HttpdSocket();
        HttpdSocket(SocketRequest &);
        HttpdSocket(int);
        virtual bool close();
        const SocketRequest & info();
        virtual size_t read(std::string & _dst,size_t size = 0 );
        virtual size_t write(std::string & _content,size_t size = 0);
        bool should_close();
        virtual int status();
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
    protected:
        char tmp[2048];
        SocketRequest _src;
        int st;
        int _hc;
        char * last_err;
    };
}

#endif
