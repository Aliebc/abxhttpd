#include "include/Httpd.hxx"
#include "include/config.hxx"
#include "include/MacroFunctions.hxx"
#include <cstring>
#include <string>


int global_argc=0;
const char ** global_argv=NULL;
const CmdArray * global_argu=NULL;

namespace abxhttpd{

int Httpd::verbose=0;
int Httpd::info_color=0;

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

HttpdCoreAddress HttpdCoreAddressTable[ABXHTTPD_CORE_MAX];

core_t HttpdCoreAddressCount=0;

HttpdCore_R MainCore(DefaultHttpdCoreAddress);

std::string ShowHttpdCoreAddressTable(char sep){
    std::stringstream _ret;
    for(core_t _i=0;_i+1<HttpdCoreAddressCount;_i++){
        _ret<<HttpdCoreAddressTable[_i].Symbol <<sep;
    }
    
    _ret<<HttpdCoreAddressTable[HttpdCoreAddressCount-1].Symbol;
    return _ret.str();
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
        HttpdCoreAddressCount++;
}

HttpdCore_R::HttpdCore_R(const HttpdCoreAddress & src){
    RegisterHttpdCore(src);
}
HttpdCore_R::~HttpdCore_R(){}

Logger * Httpd::success_logger = NULL;
Logger * Httpd::except_logger = NULL;

Httpd::Httpd(const HttpdCore & _core, const HttpdSettingList & _set){
    this->Core=_core;
    this->MCore.Handler=this->Core.Handler;
    this->MCore.IFilter=this->Core.IFilter;
    this->MCore.OFilter=this->Core.OFilter;
    this->Setting=_set;
    if(Setting.Thread_S.Log!=NULL){
        success_logger=new Logger(_set.Thread_S.Log);
    }else{
        success_logger=new Logger(Logger::FILE_FLAG::STDOUT);
    }
    if(Setting.Thread_S.Error!=NULL){
        except_logger=new Logger(_set.Thread_S.Error);
    }else{
        except_logger=new Logger(Logger::FILE_FLAG::STDERR);
    }
    if(Setting.Socket_S.IPVer!=4&&Setting.Socket_S.IPVer!=6){
        Setting.Socket_S.IPVer=4;
    }
    if(Setting.Socket_S.IPVer==4&&Setting.Socket_S.IPStr==NULL){
        Setting.Socket_S.IPStr="0.0.0.0";
    }else if(Setting.Socket_S.IPVer==6&&Setting.Socket_S.IPStr==NULL){
        Setting.Socket_S.IPStr="::";
    }
    if(Setting.Socket_S.MaxConnect<=0){
        Setting.Socket_S.MaxConnect=ABXHTTPD_CONNECT_MAX;
    }
}

Httpd::~Httpd(){
    #ifdef ABXHTTPD_WINDOWS
    WSACleanup();
    #endif 
}

void Httpd::env(){
    #ifdef ABXHTTPD_UNIX
    signal(SIGPIPE, SIG_IGN);
    #endif
    #ifdef ABXHTTPD_WINDOWS
    WSADATA wsadata;
    int wsa_c=WSAStartup(MAKEWORD(2,2),&wsadata);
    if(wsa_c!=0){
        throw BasicException("Cannot start WSA!");
    }
    #endif
}

httpd_t Httpd::start(){
    httpd_t _r=0;
    env();
    Module::init();
    Setting.Thread_S.Running=true;
    int _sk=Core.Initializer(Setting.Socket_S);
    this->Setting.Thread_S.SocketMainID=_sk;
    this->Core.Controller(Setting.Thread_S,MCore,(void *)&Setting);
    return _r;
}

httpd_t Httpd::status() const {
    return _status;
}

httpd_t Httpd::stop(){
    int st=Core.Destructor(Setting.Socket_S);
    Setting.Thread_S.Running=false;
    if(st==0){
        ABXHTTPD_INFO_PRINT(1,"[Core]Shutted down.");
        return 0;
    }else{
        return 1;
    }
}

SSMap Httpd::ExtraSettingList;

void Httpd::SetExtraSetting(const char * _key, const char * _value){
    ExtraSettingList[_key]=_value;
}

const char * Httpd::GetExtraSetting(const char * _key){
    if(ExtraSettingList.find(_key)!=ExtraSettingList.end()){
        return ExtraSettingList[_key].c_str();
    }
    return NULL;
}

}
