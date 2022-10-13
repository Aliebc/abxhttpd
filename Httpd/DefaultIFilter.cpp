#include "include/abxhttpd.H"

namespace abxhttpd
{
    HttpRequest _IStreamFilter (std::string & _src, void * _arg){
        HttpRequest _r(_src);
        _r.parse();
        return _r;
    }
    IStreamFilter DefaultIFilter=_IStreamFilter;
}