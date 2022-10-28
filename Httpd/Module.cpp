#include "include/Module.hxx"
#include "include/Httpd.hxx"
#include "include/Platform.hxx"
#include "include/config.hxx"
#include <sstream>
#include <iostream>
#include <initializer_list>
#ifdef ABXHTTPD_WINDOWS
#include <Windows.h>
#endif

namespace abxhttpd{
    ConfigureInfo * Module::_ConfData[ABXHTTPD_MAX_MODULE]{};
    module_t Module::ModuleCount=0;

    void Module::RegisterModule(ConfigureInfo * _info){
        if(ModuleCount<ABXHTTPD_MAX_MODULE){
            _ConfData[ModuleCount]=_info;
            ModuleCount++;
        }
    }

    Module::Module(ConfigureInfo * _info, void *(*dfunc)(void *), void * dta){
        dfunc(dta);
        RegisterModule(_info);
    }

    std::string Module::ShowModules(char sep){
        std::stringstream _ret;
        for(module_t _i=0;_i<ModuleCount-1;_i++){
            _ret<< _ConfData[_i]->Name <<sep;
        }
        _ret<< _ConfData[ModuleCount-1]->Name;
        return _ret.str();
    }

    template <class _A,class _B>
    std::stringstream & ModuleHTML_PAIR(std::stringstream & _ret,_A & t1,_B && t2){
        _ret<< "<tr><td class=\"left\">" << t1 << "</td><td class=\"right\">";
        _ret<< t2 << "</td></tr>";
        _ret<<std::endl;
        return _ret;
    }

    std::string Get_StartArgu(){
        std::stringstream _ret;
        if(global_argv!=NULL){
            for(int i=0;i<global_argc;i++){
                _ret << global_argv[i] << " ";
            }
        }else{
            _ret << "[Internal]";
        }
        return _ret.str();
    }

    std::string Get_OS(){
        #ifdef ABXHTTPD_WINDOWS
        char os[1024] = { 0 };
        DWORD y = sizeof(os);
        RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", REG_SZ, NULL, &os, &y);
        return os;
        #else
        struct utsname x;
        uname(&x);
        std::stringstream uts;
        uts <<x.sysname<<" " <<x.release <<" " << x.version << " " << x.machine;
        return uts.str();
        #endif
    }

    std::string Module::ShowModules_HTML(HttpRequest * _src){
        std::stringstream _ret;
        std::string arg;
        _ret<< "<div class=\"sub-title\"> core </div>"<<std::endl;
        _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
        ModuleHTML_PAIR(_ret,"Current OS",Get_OS());
        ModuleHTML_PAIR(_ret,"Start Arguments",Get_StartArgu());
        ModuleHTML_PAIR(_ret,"Registered Cores",ShowHttpdCoreAddressTable(','));
        ModuleHTML_PAIR(_ret,"Registered Modules",ShowModules(','));
        _ret<< "</table></div>" <<std::endl;
        for(module_t _i=0;_i<ModuleCount;_i++){
            _ret<< "<div class=\"sub-title\">" << _ConfData[_i]->Name <<"</div>"<<std::endl;
            _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
            _ret<<std::endl;
            for(int _j=0;_ConfData[_i]->Conf[_j].info!=NULL&&_j<ABXHTTPD_MAX_MODULE;_j++){
                ModuleHTML_PAIR(_ret,_ConfData[_i]->Conf[_j].name,_ConfData[_i]->Conf[_j].info);
            }
            _ret<< "</table></div>" <<std::endl;
        }
        if(_src!=NULL){
            _ret<< "<div class=\"sub-title\"> Abxhttpd Variables </div>"<<std::endl;
            _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
            ModuleHTML_PAIR(_ret,"REMOTE_ADDR",_src->remote_addr());
            for(auto _i=_src->headers().begin(); _i!=_src->headers().end();_i++){
                _ret<< "<tr><td class=\"left\">HTTP_"+_i->first << "</td><td class=\"right\">";
                _ret<< _i->second << "</td></tr>";
                _ret<<std::endl;
            }
            _ret<< "</table></div>" <<std::endl;
            _ret<<std::endl;
        }
        return _ret.str();
    }

    Module::Module(ConfigureInfo * _info)
    {
        RegisterModule(_info);
    }
    
    Module::~Module(){}
}
