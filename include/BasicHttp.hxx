#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

#include <stdio.h>
#include <string>
#include <map>
#include <utility>
#include "BasicStream.hxx"
#include "BasicException.hxx"

#ifdef ABXHTTPD_WINDOWS
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

namespace abxhttpd{

struct Casecmp{
    bool operator()(const std::string & s1, const std::string & s2) const{
        return strcasecmp(s1.c_str(), s2.c_str())<0;
    }
};
typedef std::map <std::string, std::string, Casecmp> HttpHeaders;
typedef std::map<std::string,std::string> SSMap;

class ABXHTTPD_API BasicHttpException{
protected:
    int except_id;
    const char * err_msg;
public:
    BasicHttpException(int);
    BasicHttpException(int,const char *);
    ~BasicHttpException();
    int code() const;
    const char * what () const noexcept;
};

class ABXHTTPD_API BasicHttp{
private:
protected:
    int status_id;
    size_t Length;
    std::string null_str;
    HttpHeaders Headers;
    std::string Buffer;
    std::string protocol;
public:
    enum S_ID{
        NOT_FINISHED=1,
        UNKNOWN_PROTOCOL,
        INVALID_HEADER,
        INVALID_FORMAT
    };
    BasicHttp();
    virtual ~BasicHttp();
    void append(const std::string & source);
    bool is_header(const std::string &) const;
    const std::string & header(const std::string &) const;
    void header(const std::string &,const std::string &&);
    void header(const std::string &,const std::string &);
    const HttpHeaders & headers() const;
    int status() const;
};

class HttpResponse;
class HttpRequest;

class BasicHttpFilter:BasicFilter{
protected:
    HttpRequest * Request;
    HttpResponse * Response;
    //HttpHandler Handler;
    BasicStream * tmp_stream;
    std::string head_tmp;
    //virtual size_t HttpHandler();
    virtual size_t StreamFilter(BasicStream &, BasicStream &, size_t) override;
public:
    BasicHttpFilter(BasicStream & src, BasicStream & dst);
    //virtual size_t exec(size_t);
};

}

#endif
