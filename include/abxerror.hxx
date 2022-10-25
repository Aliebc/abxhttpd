#ifndef ABX_ERROR_H
#define ABX_ERROR_H
#include <stdexcept>
#include <string>
#include <sstream>
#include "HttpCode.hxx"

namespace abxhttpd
{
    class abxhttpd_error:public std::runtime_error
    {
    public:
        abxhttpd_error(const std::string & _e):std::runtime_error(_e){};
        abxhttpd_error(const char * _e):std::runtime_error(_e){};
    };

    class abxhttpd_error_http
    {
    private:
        int error_code;
        const char * error_msg;
        std::string err_html;
    public:
        abxhttpd_error_http(int error_c){
            error_code=error_c;
            error_msg=HttpCodeStatus(error_c).c_str();
            err_html=HttpCodeStatusHTML(error_code);
        }
        abxhttpd_error_http(int error_c,const char * _e):abxhttpd_error_http(error_c){
            error_msg=_e;
        }
        const int & code(){
            return error_code;
        }
        const char * what(){
            return error_msg;
        }
        const std::string & html(){
            return err_html;
        }
    };
}
#endif
