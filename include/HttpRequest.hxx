#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

#include "BasicException.hxx"
#include "BasicHttp.hxx"
#include "HttpdTools.hxx"
#include "HttpdSession.hxx"
#include "HttpCode.hxx"

using std::string;

namespace abxhttpd{

class ABXHTTPD_API HttpRequest:public BasicHttp
{
private:
    string Method;
    string Path;
    string QueryString;
    string Protocol;
    SSMap GET;
    SSMap POST;
    SSMap REQUEST;
    SSMap ServerVariables;
    SSMap COOKIE;
    SVMap * SESSION;
    string Body;
public:
    HttpRequest(const char * _src,size_t _len);
    explicit HttpRequest(const string & _src);
    explicit HttpRequest(const char * _src);
    HttpRequest();
    HttpRequest(const HttpRequest & _src);
    int parse_header();
    int parse_body();
    size_t size() const override;
    const string & method() const;
    const string & request(const string &&) const;
    SSMap & cookies();
    const string & cookie(const string &&) const;
    void variables(const string & _key,const string & _val);
    void variables(const string & _key,const string && _val);
    const string & variables(const string &&) const;
    const SSMap & variables() const;
    SessionPtr * session(const string &&) const;
    SessionPtr * global(const string &&) const;
    const string & path() const;
    string & query_string();
    const string & body() const;
    string & protocol();
    const string & raw() const;
    string & rebuild();
    void clear();
    template <class Tp>
    inline Tp & sessionA(const string && _key) const{
        return session(std::move(_key))->cast<Tp>();
    }
    template <class Tp>
    inline Tp & globalA(const string && _key) const{
        return global(std::move(_key))->cast<Tp>();
    }
    ~HttpRequest();
};
}
#endif
