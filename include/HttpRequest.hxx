#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

#include "BasicException.hxx"
#include "BasicHttp.hxx"
#include "HttpdTools.hxx"
#include "HttpdSession.hxx"
#include "HttpCode.hxx"

#ifdef DEBUG

#define HttpRequest_Parser_Error(_p,_s) char errx[1024];\
snprintf(errx,1024,"HttpRequest Parser Error: At pointer %zu(%s).\
\n(Debug info:at %d lines of %s)",_p,_s,__LINE__,__FILE__);\
throw HttpException(400,errx);

#else

#define HttpRequest_Parser_Error(_p,_s) char errx[1024];\
snprintf(errx,1024,"HttpRequest Parser Error: At pointer %ld(%s).",_p,_s);\
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
    SSMap GET;
    SSMap POST;
    SSMap REQUEST;
    SSMap ServerVariables;
    SSMap COOKIE;
    SVMap * SESSION;
    std::string Body;
public:
    HttpRequest(const char * _src,size_t _len);
    explicit HttpRequest(const std::string & _src);
    explicit HttpRequest(const char * _src);
    HttpRequest();
    HttpRequest(const HttpRequest & _src);
    int parse_header();
    int parse_body();
    const std::string & method() const;
    const std::string & request(const std::string &&) const;
    SSMap & cookies();
    const std::string & cookie(const std::string &&) const;
    void variables(const std::string & _key,const std::string & _val);
    void variables(const std::string & _key,const std::string && _val);
    const std::string & variables(const std::string &&) const;
    const SSMap & variables() const;
    SessionPtr * session(const std::string &&) const;
    SessionPtr * global(const std::string &&) const;
    const std::string & path() const;
    std::string & query_string();
    const std::string & body() const;
    std::string & protocol();
    const std::string & raw() const;
    std::string & rebuild();
    void clear();
    template <class Tp>
    inline Tp & sessionA(const std::string && _key) const{
        return session(std::move(_key))->cast<Tp>();
    }
    template <class Tp>
    inline Tp & globalA(const std::string && _key) const{
        return global(std::move(_key))->cast<Tp>();
    }
    ~HttpRequest();
};
}
#endif
