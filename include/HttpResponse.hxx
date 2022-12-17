#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <string>
#include <map>
#include <sstream>

#include "HttpRequest.hxx"
#include "Module.hxx"

using std::string;

namespace abxhttpd{

/**
 * @brief Http响应类
 * 
 */
class ABXHTTPD_API HttpResponse:public BasicHttp
{
private:
    int Code; ///< 状态码
    string CodeInfo; ///< 状态码信息
    string Raw; ///< 原始字符串
public:
    bool need_send_from_stream=false;
    string need_send_from_stream_src;
    HttpResponse(const char * _src,size_t _len);
    explicit HttpResponse(const char * _src);
    explicit HttpResponse(const string & _src);
    HttpResponse(const string & _src,int code);
    HttpResponse(const string & _src,int code,HttpHeaders headers);
    void change(const string & _src);
    HttpResponse();
    friend HttpResponse operator<< (const HttpResponse & _dest, string & _src);
    bool status(int _code,const string & _str);
    bool status(int _code);
    string & body(void);
    void body(const string && _src);
    void write(const string && _src);
    void write(const string & _src);
    template <class Tp>
    void writeA(Tp && _src){
        this->write(std::to_string(_src));
    }
    string raw(void);
    void set_cookie(const string & key,
                    const string & value,
                    long expires=-1,
                    const string & path="/"
                    );
    void location(string && _loc);
    ~HttpResponse();
};


typedef HttpRequest (* IStreamFilter) (const string &, void *);
typedef void (* HttpHandler) (HttpResponse&,const HttpRequest &, void *);
typedef string (* OStreamFilter) (HttpResponse &, void *);
typedef HttpRequest (* IStreamFilter) (const string &, void *);
typedef void (* HttpHandler) (HttpResponse&,const HttpRequest &, void *);
typedef string (* OStreamFilter) (HttpResponse &, void *);

}

#endif
