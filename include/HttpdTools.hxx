#ifndef HTTPD_TOOLS_H
#define HTTPD_TOOLS_H

#define ABXHTTPD_MIN(a,b) (a<b?a:b)

namespace abxhttpd {
ABXHTTPD_API size_t _FileLength(std::string & _Path);
ABXHTTPD_API std::string _FileSuffix(std::string & _Name);
ABXHTTPD_API std::string ABX_URLDecode(std::string & _src);
ABXHTTPD_API std::string ABXInfoPageHTML(HttpRequest & _req);
}
#endif
