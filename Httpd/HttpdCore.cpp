#include "include/Httpd.hxx"
#include <cstring>
#include <string>

char _errorstr[128];

namespace abxhttpd{

HttpdCore DefaultHttpdCore={
    DefaultIFilter,
    DefaultOFilter,
    DefaultSocketI,
    DefaultThreadC,
    DefaultHttpH,
};

HttpdCoreAddress DefaultHttpdCoreAddress={
    "DefaultHttpdCore","Abxhttpd's standard built-in core.",&DefaultHttpdCore
};

HttpdCoreAddress PHPCGICore={
    "PHPCGICore","Call php by CGI.",NULL
};

HttpdCoreAddress HttpdCoreAddressTable[64]={
    DefaultHttpdCoreAddress,PHPCGICore
};

core_t HttpdCoreAddressCount=2;

void RegisterCore(HttpdCore &_core){

}

std::string ShowHttpdCoreAddressTable(void){
    std::string _ret;
    for(core_t _i=0;_i<HttpdCoreAddressCount;_i++){
        _ret+=std::string(HttpdCoreAddressTable[_i].Symbol)+"\t("+std::string(HttpdCoreAddressTable[_i].Info)+")\n";
    }
    return _ret;
}

HttpdCore * FindHttpdCore(const char * _src){
    for(core_t _i=0;_i<HttpdCoreAddressCount;_i++){
        if(strcmp(_src,HttpdCoreAddressTable[_i].Symbol)==0){
            return HttpdCoreAddressTable[_i].Core;
        }
    }
    return NULL;
}

Httpd::Httpd(HttpdCore & _core, HttpdSettingList & _set){
    this->Core=_core;
    this->MCore.Handler=this->Core.Handler;
    this->MCore.IFilter=this->Core.IFilter;
    this->MCore.OFilter=this->Core.OFilter;
    this->Setting=_set;
}

Httpd::~Httpd(){}

httpd_t Httpd::start(void){
    httpd_t _r;
    int _sk=this->Core.Initializer(this->Setting.Socket_S);
    this->Setting.Thread_S.Socket_n=_sk;
    this->Core.Controller(this->Setting.Thread_S,this->MCore,(void *)&this->Setting);
    return _r;
}


}