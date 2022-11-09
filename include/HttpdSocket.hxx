#ifndef HTTPDSOCKET_H
#define HTTPDSOCKET_H

#include "abxhttpd.H"
#include "Httpd.hxx"
#include "include/BasicStream.hxx"
#ifdef ABXHTTPD_WINDOWS

#define MSG_DONTWAIT 0x0

#endif

#define ABXHTTPD_SOCK_STREAM 0
#define ABXHTTPD_BUFFER_SIZE 2048
#define ABXHTTPD_SOCK_CLOSED 0

namespace abxhttpd{
    class ABXHTTPD_API HttpdSocket:public BasicStream
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
        virtual ~HttpdSocket();
        static int __close_socket(int ad);
        friend HttpdSocket& operator<< (HttpdSocket & src, std::string & in);
        friend HttpdSocket& operator>> (HttpdSocket& src, std::string& is);
    protected:
        SocketRequest _src;
        int _hc;
        const char * last_err;
    };
}

#endif
