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
    /**
     * @brief 构造函数
     * 
     * @param _src 源字符串
     * @param _len 长度
     */
    HttpResponse(const char * _src,size_t _len);
    /**
     * @brief 构造函数
     * 
     * @param _src 源字符串
     */
    explicit HttpResponse(const char * _src);
    /**
     * @brief 构造函数
     * 
     * @param _src 源字符串
     */
    explicit HttpResponse(const string & _src);
    /**
     * @brief 构造函数
     * 
     * @param _src 源字符串
     * @param code 状态码
     */
    HttpResponse(const string & _src,int code);
    /**
     * @brief 构造函数
     * 
     * @param _src 源字符串
     * @param code 状态码
     * @param headers 给定的头信息
     */
    HttpResponse(const string & _src,int code,const HttpHeaders & headers);
    HttpResponse();
    friend HttpResponse operator<< (const HttpResponse & _dest, string & _src);
    /**
     * @brief 设置状态码与状态信息
     * 
     * @param _code 设置的状态码
     * @param _str 状态信息
     * @return true 设置成功
     * @return false 设置失败
     */
    bool status(int _code,const string & _str);
    /**
     * @brief 设置状态码
     * 
     * @param _code 设置的状态码
     * @return true 设置成功
     * @return false 设置失败
     */
    bool status(int _code);
    /**
     * @brief 获取主体信息
     * 
     * @return string& 主体信息的内容
     */
    const string & body() const;
    /**
     * @brief 
     * 
     * @param _src 
     */
    void body(const string && _src);
    /**
     * @brief 追加信息
     * 
     * @param _src 需要追加的信息
     */
    void write(const string && _src);
    /**
     * @brief 追加信息
     * 
     * @param _src 需要追加的信息
     */
    void write(const string & _src);
    /**
     * @brief 追加内容(自含内容转换)
     * 
     * 内部调用一个to_string方法，转换为字符串
     * @tparam Tp 类型
     * @param _src 源
     */
    template <class Tp>
    void writeA(Tp && _src){
        this->write(std::to_string(_src));
    }
    string raw(void);
    /**
     * @brief 设置Cookie
     * 
     * @param key 键
     * @param value 值
     * @param expires 过期时间
     * @param path Cookie路径
     */
    void set_cookie(const string & key,
                    const string & value,
                    long expires=-1,
                    const string & path="/"
                    );
    /**
     * @brief 重定向
     * 
     * @param _loc 要重定向到的网址
     */
    void location(string && _loc);
    /**
     * @brief 析构函数
     * 
     */
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
