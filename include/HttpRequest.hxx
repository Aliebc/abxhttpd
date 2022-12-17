#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

#include "BasicException.hxx"
#include "BasicHttp.hxx"
#include "HttpdTools.hxx"
#include "HttpdSession.hxx"
#include "HttpCode.hxx"

using std::string;

namespace abxhttpd{
/**
 * @brief Http请求类
 * 
 */
class ABXHTTPD_API HttpRequest:public BasicHttp
{
private:
    string Method;
    string Path;
    string QueryString;
    SSMap GET;
    SSMap POST;
    SSMap REQUEST;
    SSMap ServerVariables;
    SSMap COOKIE;
    SVMap * SESSION;
public:
    HttpRequest(const char * _src,size_t _len);
    explicit HttpRequest(const string & _src);
    explicit HttpRequest(const char * _src);
    HttpRequest();
    HttpRequest(const HttpRequest & _src);
    /**
     * @brief 解析Http头
     * 
     * @return int 状态码
     * @exception @link HttpParserException @endlink 解析器异常信息
     */
    int parse_header();
    /**
     * @brief 解析Http主体
     * 
     * @return int 0
     */
    int parse_body();
    /**
     * @brief 获取主体的长度
     * 
     * @return size_t 主体长度
     */
    size_t size() const override;
    const string & method() const;
    /**
     * @brief 根据键获取GET/POST方法的值
     * 
     * POST只支持application/x-www-form-urlencoded
     * @return const string& 字符串值
     */
    const string & request(const string && _key) const;
    /**
     * @brief 获取Cookie映射列表
     * 
     * @return SSMap& Cookie映射列表
     */
    SSMap & cookies();
    /**
     * @brief 根据键获取Cookie
     * 
     * @param _key 键
     * @return const string& 字符串值
     */
    const string & cookie(const string && _key) const;
    /**
     * @brief 设置ServerVariables(服务器全局变量)
     * 
     * @param _key 键
     * @param _val 值
     */
    void variables(const string & _key,const string & _val);
    /**
     * @brief 设置ServerVariables(服务器全局变量)
     * 
     * @param _key 键
     * @param _val 值
     */
    void variables(const string & _key,const string && _val);
    /**
     * @brief 根据键获取ServerVariable
     * 
     * @param _key 键
     * @return const string& 字符串值
     */
    const string & variables(const string && _key) const;
    /**
     * @brief 获取ServerVariables映射列表
     * 
     * @return SSMap& ServerVariables映射列表
     */
    const SSMap & variables() const;
    /**
     * @brief 获取Session
     * 
     * @param _key 键
     * @return @link SessionPtr @endlink 智能会话指针
     */
    SessionPtr * session(const string && _key) const;
    /**
     * @brief 获取全局变量
     * 
     * @param _key 键
     * @return @link SessionPtr @endlink 智能会话指针
     */
    SessionPtr * global(const string &&) const;
    /**
     * @brief 获取请求(相对)路径
     * 
     * @return const string& 字符串值
     */
    const string & path() const;
    /**
     * @brief 获取QueryString
     * 
     * @return string& QueryString
     */
    string & query_string();
    /**
     * @brief 获取主体部分值
     * 
     * @return const string& 字符串值
     */
    const string & body() const;
    /**
     * @brief 获取协议版本
     * 
     * @return string& 字符串值
     */
    string & protocol();
    /**
     * @brief 获取原生字符串
     * 
     * @return const string& 字符串值
     */
    const string & raw() const;
    /**
     * @brief 清空缓冲区
     * 
     */
    void clear();
    /**
     * @brief 根据键获取Session值(自动解引用)
     * 
     * @tparam Tp 目标指针类型
     * @param _key 键
     * @return Tp& 目标解引用
     * @warning 如果目标为空指针,抛出 @link BasicException @endlink 异常
     */
    template <class Tp>
    inline Tp & sessionA(const string && _key) const{
        return session(std::move(_key))->cast<Tp>();
    }
    /**
     * @brief 根据键获取Session值(自动解引用)
     * 
     * @tparam Tp 目标指针类型
     * @param _key 键
     * @return Tp& 目标解引用
     * @warning 如果目标为空指针,抛出 @link BasicException @endlink 异常
     */
    template <class Tp>
    inline Tp & globalA(const string && _key) const{
        return global(std::move(_key))->cast<Tp>();
    }
    ~HttpRequest();
};
}
#endif
