#include "include/Httpd.hxx"
#include "include/HttpResponse.hxx"

namespace abxhttpd
{
    std::string _OStreamFilter (HttpResponse & _src, void *){
        return _src.raw();
    }
    OStreamFilter DefaultOFilter =_OStreamFilter;
}
