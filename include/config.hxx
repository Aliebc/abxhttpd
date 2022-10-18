#ifndef ABXH_CONFIG_H
#define ABXH_CONFIG_H

#include <map>
#include <string>
#include <cstring>

typedef std::map <char,std::string> CmdArray;

extern int global_argc;
extern char ** global_argv;
extern CmdArray * global_argu;

namespace abxhttpd{

CmdArray CmdParse(int argc, const char * argv[]);

static bool CmdArrayIs(const CmdArray * _src,char _find){
    auto _i=_src->find(_find);
    return !(_i==_src->end());
}

}
#endif