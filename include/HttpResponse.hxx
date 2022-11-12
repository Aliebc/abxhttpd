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

class ABXHTTPD_API HttpResponse:public BasicHttp
{
private:
    int Code;
    std::string CodeInfo;
    std::string Protocol;
    std::string Body;
    std::string Raw;
    const char * begin;
public:
    bool need_send_from_stream=false;
    std::string need_send_from_stream_src;
    HttpResponse(const char * _src,size_t _len);
    explicit HttpResponse(const char * _src);
    explicit HttpResponse(const std::string & _src);
    HttpResponse(const std::string & _src,int code);
    HttpResponse(const std::string & _src,int code,HttpHeaders headers);
    void change(const std::string & _src);
    HttpResponse();
    friend HttpResponse operator<< (const HttpResponse & _dest, std::string & _src);
    bool status(int _code,const std::string & _str);
    bool status(int _code);
    std::string & body(void);
    void body(const std::string && _src);
    std::string raw(void);
    void set_cookie(const std::string & key,
                    const std::string & value,
                    long expires=-1,
                    const std::string & path="/"
                    );
    void location(std::string && _loc);
    ~HttpResponse();
};

}

#endif
