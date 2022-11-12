#include "include/HttpdSession.hxx"
#include "include/Module.hxx"

#include <ctime>

namespace abxhttpd{
std::map <std::string,SessionStruct> HttpdSession::sm_sheet;

ConfigureInfo AX_SESSION_INFO={
    "session",{
        {"Support","enabled"},
        {"Default Expire",ABXHTTPD_STR(ABXHTTPD_SESSION_DEAD) "s"},
        {"Key Length",ABXHTTPD_STR(ABXHTTPD_SESSION_IDLEN)},
        {"Unique",ABXHTTPD_STR(ABXHTTPD_SESSION_UNIQUE)}
    }
};

Module AX_SESSION(&AX_SESSION_INFO);

int HttpdSession::dead=ABXHTTPD_SESSION_DEAD;
void HttpdSession::set_expire(int d){
    dead=d;
}
std::string HttpdSession::allocate(){
    char tmp[ABXHTTPD_SESSION_IDLEN+1]{};
    srand(time(NULL));
    do{
        for(int i=0;i<ABXHTTPD_SESSION_IDLEN;++i){
            int p=rand() % 26;
            tmp[i]=static_cast<char>(p + ((rand()%2)?'A':'a'));
        }
    }while(ABXHTTPD_SESSION_UNIQUE&&sm_sheet.find(tmp)!=sm_sheet.end());
    sm_sheet[tmp]=SessionStruct();
    sm_sheet[tmp].update=time(NULL);
    check_expire();
    return tmp;
}

void HttpdSession::check_expire(){
    time_t now=time(NULL);
    for(auto i=sm_sheet.begin();i!=sm_sheet.end();){
        if(now-(i->second.update)>dead){
            sm_sheet.erase(i++);
        }else{
            ++i;
        }
    }
}

SVMap & HttpdSession::get(const std::string & _key){
    check_expire();
    sm_sheet[_key].update=time(NULL);
    return sm_sheet[_key].pt;
}

}
