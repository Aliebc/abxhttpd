#include "include/Httpd.hxx"
#include "include/abxhttpd.H"
#include <cstring>
#include <string>

char _errorstr[128];
int verbose=0;
int info_color=0;
int global_argc;
char ** global_argv;
CmdArray * global_argu;

namespace abxhttpd{

HttpdCore DefaultHttpdCore={
    DefaultIFilter,
    DefaultOFilter,
    DefaultSocketI,
    DefaultSocketD,
    DefaultThreadC,
    DefaultHttpH,
};

HttpdCoreAddress DefaultHttpdCoreAddress={
    "DefaultHttpdCore","Abxhttpd's standard built-in core.",&DefaultHttpdCore
};

HttpdCoreAddress PHPCGICore={
    "PHPCGICore","Call php by CGI.",NULL
};

HttpdCoreAddress HttpdCoreAddressTable[ABXHTTPD_CORE_MAX];

core_t HttpdCoreAddressCount=0;

HttpdCore_R MainC(DefaultHttpdCoreAddress);

std::string ShowHttpdCoreAddressTable(void){
    std::string _ret;
    for(core_t _i=0;_i<HttpdCoreAddressCount;_i++){
        _ret+=std::string(HttpdCoreAddressTable[_i].Symbol)+"\t("+std::string(HttpdCoreAddressTable[_i].Info)+")\n";    }
    return _ret;
}

const HttpdCore * FindHttpdCore(const char * _src){
    for(core_t _i=0;_i<HttpdCoreAddressCount;_i++){
        if(strcmp(_src,HttpdCoreAddressTable[_i].Symbol)==0){
            return HttpdCoreAddressTable[_i].Core;
        }
    }
    return NULL;
}

void RegisterHttpdCore(HttpdCoreAddress _core){
        memcpy((void *)&HttpdCoreAddressTable[HttpdCoreAddressCount],(void *)&_core,sizeof(HttpdCoreAddress));
        ABXHTTPD_DEBUG_PRINTF("[Main]Registered Core: %s\n",_core.Symbol);
        HttpdCoreAddressCount++;
}

HttpdCore_R::HttpdCore_R(HttpdCoreAddress src){
    RegisterHttpdCore(src);
}
HttpdCore_R::~HttpdCore_R(){}

Httpd::Httpd(const HttpdCore & _core, HttpdSettingList & _set){
    this->Core=_core;
    this->MCore.Handler=this->Core.Handler;
    this->MCore.IFilter=this->Core.IFilter;
    this->MCore.OFilter=this->Core.OFilter;
    this->Setting=_set;
}

Httpd::~Httpd(){}

httpd_t Httpd::start(){
    httpd_t _r=0;
    Setting.Thread_S.Is_running=true;
    int _sk=this->Core.Initializer(this->Setting.Socket_S);
    this->Setting.Thread_S.Socket_n=_sk;
    this->Core.Controller(this->Setting.Thread_S,this->MCore,(void *)&this->Setting);
    return _r;
}

httpd_t Httpd::status() const {
    return _status;
}

httpd_t Httpd::stop(){
    int st=Core.Destructor(Setting.Socket_S);
    Setting.Thread_S.Is_running=false;
    if(st==0){
        ABXHTTPD_INFO_PRINT(1,"[Core]Shutted down.");
        return 0;
    }else{
        return 1;
    }
}

}