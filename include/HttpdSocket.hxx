#ifndef HTTPDSOCKET_H
#define HTTPDSOCKET_H

#include "abxhttpd.H"
#include "Httpd.hxx"
#ifdef ABXHTTPD_WINDOWS

#define MSG_DONTWAIT 0x0

#endif

#define ABXHTTPD_SOCK_STREAM 0
#define ABXHTTPD_BUFFER_SIZE 2048

namespace abxhttpd{
    class ABXHTTPD_API HttpdSocket
    {
    public:
        HttpdSocket();
        HttpdSocket(const SocketRequest &);
        const SocketRequest & info();
        virtual bool close();
        virtual size_t read(std::string & _dst,size_t size = 0 );
        virtual size_t write(const std::string & _content,size_t size = 0);
        virtual int status();
        virtual const char * GetLastError();
        ~HttpdSocket();
        static int __close_socket(int ad);
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
        char tmp[ABXHTTPD_BUFFER_SIZE];
        SocketRequest _src;
        int st;
        int _hc;
        const char * last_err;
    };
}

#endif
