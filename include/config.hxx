#ifndef ABXH_CONFIG_H
#define ABXH_CONFIG_H

#include <map>
#include <string>
#include <cstring>

typedef std::map <char,std::string> CmdArray;

namespace abxhttpd{

static CmdArray CmdParse(int argc, const char * argv[]){
    int _p=0;
    CmdArray ret;
    while(true){
        if(*argv[_p]=='-'){
            if(strlen(argv[_p])==2){
                if(_p==argc-1){
                    ret[*(argv[_p]+1)]=std::string();
                    break;
                }else{
                    if(*argv[_p+1]=='-'){
                        ret[*(argv[_p]+1)]=std::string();
                    }else{
                        ret[*(argv[_p]+1)]=std::string(argv[_p+1]);
                        _p++;
                    }
                }
            }else if(strlen(argv[_p])>2){
                ret[*(argv[_p]+1)]=std::string(argv[_p]+2);
            }
        }
        if(_p==argc-1){
            break;
        }
        _p++;
    }
    return ret;
}

static bool CmdArrayIs(CmdArray & _src,char _find){
    CmdArray::iterator _i=_src.find(_find);
    return !(_i==_src.end());
}

}
#endif