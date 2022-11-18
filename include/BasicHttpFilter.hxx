#ifndef HTTP_FILTER_H
#define HTTP_FILTER_H

#include "BasicHttp.hxx"
#include "Httpd.hxx"

namespace abxhttpd{
class BasicHttpFilter:BasicFilter{
protected:
    HttpRequest * Request;
    HttpResponse * Response;
    HttpHandler Handler;
    BasicStream ** tmp_stream;
    //virtual size_t HttpHandler();
    size_t StreamFilter(BasicStream &, BasicStream &, size_t) override;
public:
    BasicHttpFilter(BasicStream & src, BasicStream & dst);
    void set_handler(HttpHandler handler);
    ~BasicHttpFilter();
    //virtual size_t exec(size_t);
};
}

#endif
