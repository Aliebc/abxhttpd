#ifndef HTTPCODE_H
#define HTTPCODE_H
#include <string>
#include <map>
#include "Version.hxx"

typedef std::map <int, std::string> HttpCodeL;

#define ABXHTTPD_DEFAULT_HTTPCODE 200
#define ABXHTTPD_DEFAULT_PROTOCOL "HTTP/1.1"
#define ABXHTTPD_DEFAULT_LINE "\n"

namespace abxhttpd{
extern HttpCodeL HttpCodeList;
extern const std::string & HttpCodeStatus(int _code);
std::string HttpCodeStatusHTML(int _code);

}

#endif
