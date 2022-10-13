#ifndef ABX_ERROR_H
#define ABX_ERROR_H
#include <stdexcept>

namespace abxhttpd
{
    class abxhttpd_error:public std::runtime_error
    {
    public:
        abxhttpd_error(const std::string & _e):std::runtime_error(_e){};
        abxhttpd_error(const char * _e):std::runtime_error(_e){};
    };
}
#endif