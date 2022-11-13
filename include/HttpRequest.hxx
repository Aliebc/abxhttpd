#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

#include "abxerror.hxx"
#include "BasicHttp.hxx"
#include "HttpdTools.hxx"
#include "HttpdSession.hxx"
#include "HttpCode.hxx"

#ifdef DEBUG

#define HttpRequest_Parser_Error(_p,_s) char errx[1024];\
sprintf(errx,"HttpRequest Parser Error: At pointer %zu(%s).\
\n(Debug info:at %d lines of %s)",_p,_s,__LINE__,__FILE__);\
throw HttpException(400,errx);

#else

#define HttpRequest_Parser_Error(_p,_s) char errx[1024];\
sprintf(errx,"HttpRequest Parser Error: At pointer %ld(%s).",_p,_s);\
throw HttpException(400,errx);

#endif

#define HttpRequest_Parser_Assert(_assert, _position, _error_string) \
if(!(_assert)){HttpRequest_Parser_Error(_position,_error_string)};

namespace abxhttpd{

class ABXHTTPD_API HttpRequest:public BasicHttp
{
private:
    size_t Length;
    std::string Method;
    std::string Path;
    std::string Query_String;
    std::string Protocol;
    std::string RemoteAddr;
    SSMap GET;
    SSMap POST;
    SSMap REQUEST;
    SSMap COOKIE;
    SVMap * SESSION;
    std::string Body;
    std::string Raw;
public:
    HttpRequest(const char * _src,size_t _len);
    explicit HttpRequest(const std::string & _src);
    explicit HttpRequest(const char * _src);
    HttpRequest();
    HttpRequest(const HttpRequest & _src);
    void parse(void);
    bool state;
    const std::string & method(void) const;
    const std::string & request(const std::string &&) const;
    const std::string & cookie(const std::string &&) const;
    SessionPtr * session(const std::string &&);
    template <class Tp>
    Tp & sessionA(const std::string && _key){
        return session(std::move(_key))->cast<Tp>();
    }
    std::string & path(void);
    std::string & query_string(void);
    std::string & body(void);
    std::string & protocol(void);
    const std::string & remote_addr(void);
    void remote_addr(const std::string &);
    std::string & raw(void);
    std::string & rebuild(void);
    void clear();
    ~HttpRequest();
};
}
#endif
