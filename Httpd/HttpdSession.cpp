#include "include/HttpdSession.hxx"
#include "include/Module.hxx"
#include "include/random.H"

#include <ctime>
#include <algorithm>

namespace abxhttpd{
std::map <std::string,SessionStruct> HttpdSession::sm_sheet;
SVMap HttpdSession::global;
int HttpdSession::dead=ABXHTTPD_SESSION_DEAD;

ModuleConfigure AX_SESSION_INFO={
    "session",{
        {"Support","enabled"},
        {"Default Expire",ABXHTTPD_STR(ABXHTTPD_SESSION_DEAD) "s"},
        {"Key Length",ABXHTTPD_STR(ABXHTTPD_SESSION_IDLEN)},
        {"Unique",ABXHTTPD_STR(ABXHTTPD_SESSION_UNIQUE)}
    }
};

Module AX_SESSION(&AX_SESSION_INFO);

void HttpdSession::set_expire(int d){
    dead=d;
}
std::string HttpdSession::allocate(){
    char tmp[ABXHTTPD_SESSION_IDLEN+1]={0};
    Random::Generate(tmp, ABXHTTPD_SESSION_IDLEN);
    do{
        for(int i=0;i<ABXHTTPD_SESSION_IDLEN;++i){
            tmp[i]=static_cast<char>(std::abs(tmp[i] % 26) + (tmp[i]>0?'A':'a'));
        }
    }while(ABXHTTPD_SESSION_UNIQUE&&sm_sheet.find(tmp)!=sm_sheet.end());
    sm_sheet[tmp]=SessionStruct();
    sm_sheet[tmp].update=time(NULL);
    return tmp;
}

bool HttpdSession::check_expire(const SessionStruct & session_ss){
    time_t now=time(NULL);
    if((now-session_ss.update)>dead){
        return false;
    }
    return true;
}

SVMap & HttpdSession::get(const std::string & _key){
    if(!check_expire(sm_sheet[_key])){
        sm_sheet.erase(_key);
    }
    sm_sheet[_key].update=time(NULL);
    return sm_sheet[_key].pt;
}

void SessionPtr::move(SessionPtr && s){
    inter_del();
    s.destory_unique();
    address=s.address;
    del=s.del;
    s.address=NULL;
    s.del=NULL;
}

void SessionPtr::move(SessionPtr & s){
    this->move(std::move(s));
}

bool SessionPtr::null(){
    return (address==NULL);
}

SessionPtr::SessionPtr(){
    address=NULL;
    del=NULL;
    unique=true;
}

void SessionPtr::destory(){
        inter_del();
}

void SessionPtr::destory_unique(){
    unique=false;
}

void SessionPtr::inter_del(){
    if((address!=NULL)&&(unique==true)){
        del(address);
        del=NULL;
        address=NULL;
    }
}

SessionPtr::~SessionPtr(){inter_del();}

}
