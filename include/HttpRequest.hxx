#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

#include "include/abxerror.hxx"

#ifdef DEBUG

#define HttpRequest_Parser_Error(_p,_s) char errx[1024];\
    sprintf(errx,"HttpRequest Parser Error: At pointer %ld(%s).\
    \n(Debug info:at %d lines of %s)",_p,_s,__LINE__,__FILE__);\
throw abxhttpd_error(errx);

#else

#define HttpRequest_Parser_Error(_p,_s) char errx[1024];\
    sprintf(errx,"HttpRequest Parser Error: At pointer %ld(%s).",_p,_s);\
throw abxhttpd_error(errx);

#endif

#define HttpRequest_Parser_Assert(_assert, _position, _error_string) \
if(!(_assert)){HttpRequest_Parser_Error(_position,_error_string)};

namespace abxhttpd{
typedef std::map <std::string, std::string> StrArray;

class HttpRequest
{
private:
    size_t Length;
    std::string Method;
    std::string Path;
    std::string Query_string;
    std::string Protocol;
    std::string Remote_addr;
    StrArray Headers;
    std::string Body;
    std::string Raw;
    const char * begin;
public:
    HttpRequest(const char * _src,size_t _len);
    HttpRequest(const std::string & _src);
    HttpRequest(const char * _src);
    HttpRequest(const HttpRequest & _src);
    void parse(void);
    bool state;
    StrArray & headers(void);
    bool is_header(const char * _header);
    bool is_header(const std::string & _header);
    std::string & header(const char * _header);
    std::string & header(const std::string & _header);
    std::string & method(void);
    std::string & path(void);
    std::string & query_string(void);
    std::string & body(void);
    std::string & protocol(void);
    std::string & remote_addr(void);
    std::string & raw(void);
    std::string & rebuild(void);
    ~HttpRequest();
};
}
#endif