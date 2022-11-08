#ifndef HTTPD_TOOLS_H
#define HTTPD_TOOLS_H

#define ABXHTTPD_MIN(a,b) (a<b?a:b)

namespace abxhttpd {

class HttpdTools{
public:
    static size_t _FileLength(std::string & _Path);
    static std::string _FileSuffix(const std::string & _Name);
    static std::string ABX_URLDecode(std::string & _src);
    static std::string ABXInfoPageHTML(HttpRequest & _req);
    HttpdTools();
    ~HttpdTools();
};
ABXHTTPD_API std::string ABXInfoPageHTML(HttpRequest & _req);
}
#endif
