#ifndef ABXH_CONFIG_H
#define ABXH_CONFIG_H

#include <map>
#include <string>
#include <cstring>

typedef std::map <char,std::string> CmdArray;

ABXHTTPD_API extern int global_argc;
ABXHTTPD_API extern const char ** global_argv;
ABXHTTPD_API extern const CmdArray * global_argu;

namespace abxhttpd{

ABXHTTPD_API CmdArray CmdParse(int argc, const char * argv[]);

inline bool CmdArrayIs(const CmdArray * _src,char _find){
    auto _i=_src->find(_find);
    return !(_i==_src->end());
}

}
#endif
