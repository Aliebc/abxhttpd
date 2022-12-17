#ifndef HTTP_FILTER_H
#define HTTP_FILTER_H

#include "BasicHttp.hxx"
#include "Httpd.hxx"
#include "include/BasicStream.hxx"
namespace abxhttpd{
/**
 * @brief 基础HTTP过滤器类
 * 
    基础HTTP过滤器类, 他的功能是....
 */
class BasicHttpFilter:public BasicFilter{
protected:
    /**
     * @brief 请求
     * 
     * 这个请求的详细内容是...
     */
    HttpRequest * Request; 
    HttpResponse * Response; ///< 响应
    HttpHandler Handler;
    FileStream * tmp_fstream;
    bool send_from_stream;
    SocketRequest Iset;
    //virtual size_t HttpHandler();
    size_t StreamFilter(BasicStream &, BasicStream &, size_t) override;
public:
    BasicHttpFilter(BasicStream & src, BasicStream & dst);
    void set(HttpHandler handler,SocketRequest &);
    ~BasicHttpFilter();
    BasicHttpFilter(const BasicHttpFilter &)=delete;
    //virtual size_t exec(size_t);
};
}

#endif
