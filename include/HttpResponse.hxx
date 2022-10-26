#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <string>
#include <map>
#include <sstream>

#include "HttpRequest.hxx"
#include "Module.hxx"

#ifdef DEBUG

#define HttpResponse_Parser_Error(_p,_s) char errx[1024];\
    sprintf(errx,"HttpResponse Error: At pointer %ld(%s).\
    \n(Debug info:at %d lines of %s)",_p,_s,__LINE__,__FILE__);\
throw abxhttpd_error(errx);

#else

#define HttpResponse_Parser_Error(_p,_s) char errx[1024];\
    sprintf(errx,"HttpResponse Error: At pointer %ld(%s).",_p,_s);\
throw abxhttpd_error(errx);

#endif

#define HttpResponse_Parser_Assert(_assert, _position, _error_string) \
if(!(_assert)){HttpResponse_Parser_Error(_position,_error_string)};

namespace abxhttpd{

typedef std::map <std::string, std::string> StrArray;

class HttpResponse
{
private:
    int Code;
    std::string CodeInfo;
    std::string Protocol;
    StrArray Headers;
    std::string Body;
    std::string Raw;
    const char * begin;
public:
    bool need_send_from_stream=false;
    std::string need_send_from_stream_src;
    HttpResponse(const char * _src,size_t _len);
    HttpResponse(const char * _src);
    HttpResponse(std::string & _src);
    HttpResponse();
    friend HttpResponse operator<< (const HttpResponse & _dest, std::string & _src);
    bool status(int _code,const std::string & _str);
    bool status(int _code);
    std::string & header(const char * _h);
    void header(const char * _h,std::string && _src);
    std::string & header(const std::string & _h);
    std::string & body(void);
    void body(const std::string && _src);
    std::string raw(void);
    ~HttpResponse();
};

}

#endif
