#ifndef ABX_ERROR_H
#define ABX_ERROR_H
#include <stdexcept>
#include <string>
#include <sstream>
#include "HttpCode.hxx"

namespace abxhttpd
{
    /**
     *@brief 异常基类
     *
     *本类继承了std::runtime_error，作为Abxhttpd的异常基类，其余的异常都通过此类继承，确保异常的统一性
     */
    class BasicException:public std::runtime_error
    {
    public:
        ///STL字符串构造法
        explicit BasicException(const std::string & _e):std::runtime_error(_e){};
        ///C字符串构造法
        explicit BasicException(const char * _e):std::runtime_error(_e){};
        ~BasicException()=default;
    };
    
    /**
     *@brief HTTP解析异常类
     *
     *本类在构造的基础上自动生成http错误信息，可以直接返回
     */
class HttpException:public BasicException
{
private:
    int error_code; ///<存储异常的http状态码
    std::string except_str; ///< 存储异常的文字信息
    std::string err_html; ///< 存储异常的HTML信息
public:
    explicit HttpException(int error_c):BasicException(""){
        error_code=error_c;
        err_html=HttpCodeStatusHTML(error_code);
    }
    HttpException(int error_c,const char * _e):HttpException(error_c){
        except_str=_e;
    }
    ///获取HTTP状态码
    int code() const{
        return error_code;
    }
    ///获取错误信息
    const char * what() const noexcept override{
        return except_str.c_str();
    }
    ///获取错误的HTML输出
    std::string html() const{
        return err_html;
    }
    /*HttpResponse response() const{
        HttpResponse tmp(err_html,error_code);
        if(error_code==302){
            //tmp.location(except_str);
        }
        return tmp;
    }*/
};
}
#endif
