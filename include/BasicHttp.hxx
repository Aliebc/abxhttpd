#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

#include <stdio.h>
#include <string>
#include <map>
#include "BasicStream.hxx"

enum HttpInternalStatus
{
    ABXHTTPD_HTTP_CLOSED=0,
    ABXHTTPD_HTTP_CONTINUE_RECV=1<<0,
    ABXHTTPD_HTTP_CONTINUE_WRITE=1<<1,
    ABXHTTPD_HTTP_FINISHED_RECV=1<<2,
    ABXHTTPD_HTTP_FINISHED_WRITE=1<<3,
    ABXHTTPD_HTTP_HANDLING=1<<4,
    ABXHTTPD_HTTP_KEEP_ALIVE=1<<5
};

namespace abxhttpd{
typedef std::map <std::string, std::string> HttpHeaders;

class BasicHttpException{
protected:
    int except_id;
    const char * err_msg;
public:
    BasicHttpException(int);
    BasicHttpException(int,const char *);
    ~BasicHttpException();
    int id() const;
    const char * what () const;
};

class BasicHttp{
protected:
    int status_id;
    size_t Length;
    HttpHeaders Headers;
    std::string Buffer;
    std::string protocol;
public:
    BasicHttp();
    virtual ~BasicHttp();
    //virtual
    bool is_header(const char *) const;
    bool is_header(const std::string &) const;
    const std::string & header(const std::string &) const;
    const std::string & header(const char *) const;
    void header(const std::string &,const std::string &);
    void header(const char *,const std::string &);
    int status() const;
    int status(int);
};

class HttpResponse;
class HttpRequest;

class BasicHttpFilter:BasicFilter{
protected:
    HttpRequest * Request;
    HttpResponse * Response;
public:
    BasicHttpFilter();
    virtual size_t exec(size_t);
};

}

#endif
