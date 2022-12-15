#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

#include <stdio.h>
#include <string>
#include <map>
#include <utility>
#include "BasicStream.hxx"
#include "BasicException.hxx"

using std::string;

#ifdef ABXHTTPD_WINDOWS
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

namespace abxhttpd{

struct Casecmp{
    bool operator()(const string & s1, const string & s2) const{
        return strcasecmp(s1.c_str(), s2.c_str())<0;
    }
};

/// HttpHeaders 专用(比较函数不分大小写)
typedef std::map <string, string, Casecmp> HttpHeaders;
/// string->string 的映射
typedef std::map<string,string> SSMap;

class ABXHTTPD_API HttpParserException{
protected:
    int except_id;
    const char * err_msg;
public:
    HttpParserException(int);
    HttpParserException(int,const char *);
    ~HttpParserException();
    int code() const;
    const char * what () const noexcept;
};

class ABXHTTPD_API BasicHttp{
private:
protected:
    int status_id;
    size_t Length;
    string null_str;
    HttpHeaders Headers;
    string Buffer;
    string protocol;
public:
    enum S_ID{
        NOT_FINISHED=1,
        UNKNOWN_PROTOCOL,
        INVALID_HEADER,
        INVALID_FORMAT
    };
    BasicHttp();
    virtual ~BasicHttp();
    void append(const string & source);
    bool is_header(const string &) const;
    size_t length() const;
    virtual size_t size() const;
    const string & header(const string &) const;
    void header(const string &,const string &&);
    void header(const string &,const string &);
    const HttpHeaders & headers() const;
    int status() const;
};

class HttpResponse;
class HttpRequest;



}

#endif
