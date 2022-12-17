#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

#include <stdio.h>
#include <string>
#include <map>
#include <utility>
#include "BasicStream.hxx"
#include "BasicException.hxx"

using std::string;

#ifdef ABXHTTPD_WINDOWS
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

namespace abxhttpd{

/**
 * @brief 比较类(不分大小写)
 * 
 * 不分大小的比较字符，主要用于Http的头信息解析，因为http的头信息不区分大小写
 */
struct Casecmp{
    bool operator()(const string & s1, const string & s2) const{
        return strcasecmp(s1.c_str(), s2.c_str())<0;
    }
};

/// HttpHeaders 专用(比较函数不分大小写)
typedef std::map <string, string, Casecmp> HttpHeaders;
/// string->string 的映射
typedef std::map<string,string> SSMap;

/**
 * @brief 解析器异常类
 * 
 */
class ABXHTTPD_API HttpParserException{
protected:
    int except_id;
    const char * err_msg;
public:
    HttpParserException(int);
    HttpParserException(int,const char *);
    ~HttpParserException();
    /**
     * @brief 获取错误码
     * 
     * @return int 错误码, 同 @link BasicHttp::S_ID @endlink 相比较
     */
    int code() const;
    const char * what () const noexcept;
};

/**
 * @brief Http协议基类
 * 
 * 由于Http协议发送和接受的相似性,故写一个Http协议的基类,由此派生出请求类和响应类
 */
class ABXHTTPD_API BasicHttp{
private:
protected:
    int status_id; ///<状态码
    /**
     * @brief 长度
     * @warning 注意是Content-Length的长度
     */
    size_t Length;
    string null_str; ///< 空字符串
    HttpHeaders Headers; ///< Http请求头
    string Buffer; ///< 缓冲区
    string Protocol; ///< 协议版本
    std::string Body; ///< 主体信息
public:
    ///状态信息
    enum S_ID{
        NOT_FINISHED=1, ///< 未完成
        UNKNOWN_PROTOCOL, ///< 未知协议（错误）
        INVALID_HEADER, ///< 无效请求头（错误）
        INVALID_FORMAT ///< 无效格式（错误）
    };
    BasicHttp();
    virtual ~BasicHttp();
    /**
     * @brief 追加字符串到缓冲区
     * 
     * @param source 追加字符串
     */
    void append(const string & source);
    /**
     * @brief 判断给定键的请求头是否存在
     * 
     * @param key 键
     * @return true 存在
     * @return false 不存在
     */
    bool is_header(const string & key) const;
    /**
     * @brief 获得长度
     * 
     * @return size_t 长度
     */
    size_t length() const;
    /**
     * @brief 获得Body长度
     * 
     * @return size_t 长度
     */
    virtual size_t size() const;
    /**
     * @brief 获取给定键的请求头的值
     * 
     * @return const string& 请求头的值(引用,不可直接修改)
     */
    const string & header(const string & key) const;
    /**
     * @brief 修改请求头的值
     * 
     * @param key 键
     * @param value 值
     * @warning 不存在键会直接创建
     */
    void header(const string & key,const string && value);
    /**
     * @brief 修改请求头的值
     * 
     * @param key 键
     * @param value 值
     * @warning 不存在键会直接创建
     */
    void header(const string & key,const string & value);
    /**
     * @brief 直接获取头信息的映射表
     */
    const HttpHeaders & headers() const;
    /**
     * @brief 返回当前状态码
     * 
     * @return int 状态码
     * @warning 需要和 @link S_ID @endlink 中的值相比较
     */
    int status() const;
};

class HttpResponse;
class HttpRequest;



}

#endif
