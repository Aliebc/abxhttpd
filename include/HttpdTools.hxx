#ifndef HTTPD_TOOLS_H
#define HTTPD_TOOLS_H

#define ABXHTTPD_MIN(a,b) (a<b?a:b)

#include "HttpRequest.hxx"
#include "BasicException.hxx"

namespace abxhttpd {
/**
 * @brief 工具类
 *
 * 本类中集合了一些工具，在各类情形中都可以用到，由于本类的特殊性，因此所有函数均为共有静态成员函数
 */
class ABXHTTPD_API HttpdTools{
public:
    /**
     @brief 获取文件长度
     @param _Path 文件路径
     */
    static size_t _FileLength(std::string & _Path);
    /**
     @brief 获取文件后缀名
     @param _Name 文件名
     */
    static std::string _FileSuffix(const std::string & _Name);
    /**
     @brief URL解码
     @param _src 源URL
     */
    static std::string ABX_URLDecode(const std::string & _src);
    /**
     @brief 输出信息页面
     @param _req 源请求
     */
    static std::string ABXInfoPageHTML(const HttpRequest & _req);
    static void ParseQueryString(SSMap & ssm, const std::string & _src) noexcept;
    static void ParseCookie(SSMap & ssm, const std::string & _src) noexcept;
    /// 默认构造函数
    HttpdTools();
    /// 默认析构函数
    ~HttpdTools();
};
}
#endif
