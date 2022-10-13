#ifndef MIME_H
#define MIME_H

#include <map>
#include <string>

namespace abxhttpd{

typedef std::map <std::string,std::string> MIMEs;
extern MIMEs MIMEList;
std::string _GMIME(std::string & _suffix);

}

#endif