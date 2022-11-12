#include "include/Httpd.hxx"
#include "include/HttpRequest.hxx"

namespace abxhttpd
{
    HttpRequest _IStreamFilter (const std::string & _src, void * _arg){
        HttpRequest _r(_src);
        _r.parse();
        return _r;
    }
    IStreamFilter DefaultIFilter=_IStreamFilter;
}
