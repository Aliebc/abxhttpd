#ifndef HTTPCODE_H
#define HTTPCODE_H
#include <string>
#include <map>

typedef std::map <int, std::string> HttpCodeL;

extern HttpCodeL HttpCodeList;

#define ABXHTTPD_DEFAULT_HTTPCODE 200
#define ABXHTTPD_DEFAULT_PROTOCOL "HTTP/1.1"
#define ABXHTTPD_DEFAULT_LINE "\n"

extern const std::string & HttpCodeStatus(int _code);

#endif