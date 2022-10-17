#include "include/Module.hxx"
#include "include/Platform.hxx"
extern int global_argc;
extern char ** global_argv;
#include <sstream>
#include <iostream>
#include <initializer_list>
#ifdef ABXHTTPD_WINDOWS
#include <Windows.h>
#endif

namespace abxhttpd{
    ConfigureInfo _ConfData[ABXHTTPD_MAX_MODULE];
    module_t ModuleCount=0;

    void RegisterModule(ConfigureInfo _info){
        ConfigureInfo _cof;
        if(ModuleCount<ABXHTTPD_MAX_MODULE){
            _ConfData[ModuleCount]=_info;
            ModuleCount++;
        }
    }

    std::string ShowModules(){
        std::stringstream _ret;
        for(module_t _i=0;_i<ModuleCount;_i++){
            _ret<< _ConfData[_i].Name <<std::endl;
        }
        return _ret.str();
    }

    template <class _A,class _B>
    std::stringstream & ModuleHTML_PAIR(std::stringstream & _ret,_A & t1,_B & t2){
        _ret<< "<tr><td class=\"left\">" << t1 << "</td><td class=\"right\">";
        _ret<< t2 << "</td></tr>";
        _ret<<std::endl;
        return _ret;
    }

    std::string Get_StartArgu(){
        std::stringstream _ret;
        for(int i=0;i<global_argc;i++){
            _ret << global_argv[i] << " ";
        }
        return _ret.str();
    }

    std::string Get_OS(){
        #ifdef ABXHTTPD_WINDOWS
        char x[1024] = { 0 };
        DWORD y = sizeof(x);
        RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", REG_SZ, NULL, &x, &y);
        return std::string(x);
        #else
        struct utsname x;
        uname(&x);
        return std::string(x.sysname)+std::string(" ")+std::string(x.version);
        #endif
    }

    std::string ShowModules_HTML(HttpRequest * _src){
        std::stringstream _ret;
        std::string arg;
        _ret<< "<div class=\"sub-title\">" << "System" <<"</div>"<<std::endl;
        _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
        arg=Get_OS();
        ModuleHTML_PAIR(_ret,"Current OS",arg);
        arg=Get_StartArgu();
        ModuleHTML_PAIR(_ret,"Start Arguments",arg);
        _ret<< "</table></div>" <<std::endl;
        for(module_t _i=0;_i<ModuleCount;_i++){
            _ret<< "<div class=\"sub-title\">" << _ConfData[_i].Name <<"</div>"<<std::endl;
            _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
            _ret<<std::endl;
            for(int _j=0;_ConfData[_i].Conf[_j].info!=NULL&&_j<ABXHTTPD_MAX_MODULE;_j++){
                ModuleHTML_PAIR(_ret,_ConfData[_i].Conf[_j].name,_ConfData[_i].Conf[_j].info);
            }
            _ret<< "</table></div>" <<std::endl;
        }
        if(_src!=NULL){
            _ret<< "<div class=\"sub-title\"> Abxhttpd Variables </div>"<<std::endl;
            _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
            ModuleHTML_PAIR(_ret,"REMOTE_ADDR",_src->remote_addr());
            for(auto _i=_src->headers().begin(); _i!=_src->headers().end();_i++){
                _ret<< "<tr><td class=\"left\">" << std::string("HTTP_")+_i->first << "</td><td class=\"right\">";
                _ret<< _i->second << "</td></tr>";
                _ret<<std::endl;
            }
            _ret<< "</table></div>" <<std::endl;
            _ret<<std::endl;
        }
        return _ret.str();
    }

    Module::Module(ConfigureInfo _info)
    {
        RegisterModule(_info);
    }
    
    Module::~Module(){}

}