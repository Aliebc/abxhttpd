#ifndef MIME_H
#define MIME_H

#include <map>
#include <string>

namespace abxhttpd{

typedef const std::map <std::string,std::string> MIMEs;
extern MIMEs MIMEList;
std::string _GMIME(const std::string & _suffix);
std::string _GMIME(const char * _suffix);

}

#endif
