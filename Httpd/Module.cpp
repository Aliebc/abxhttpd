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
    ModuleConfigure * Module::_ConfData[ABXHTTPD_MAX_MODULE]{};
    module_t Module::ModuleCount=0;

    void Module::RegisterModule(ModuleConfigure * _info){
        if(ModuleCount<ABXHTTPD_MAX_MODULE){
            _ConfData[ModuleCount]=_info;
            ModuleCount++;
        }
    }

    void Module::init(){
        for(module_t _i=0;_i<ModuleCount;_i++){
            if(_ConfData[_i]->dfunc!=NULL){
                _ConfData[_i]->dfunc(_ConfData[_i]->data);
            }
        }
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
        if(Httpd::global_argv!=NULL){
            for(int i=0;i<Httpd::global_argc;i++){
                _ret << Httpd::global_argv[i] << " ";
            }
        }else{
            _ret << "[Internal]";
        }
        return _ret.str();
    }

    /// 获取系统版本
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
    /**
     * @brief 拼接模块显示列表(HTML)
     * 
     * @param _src Http请求
     * @return std::string 拼接好的字符串
     */
    std::string Module::ShowModules_HTML(const HttpRequest * _src){
        std::stringstream _ret;
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
            for(int _j=0;_j<ABXHTTPD_MAX_MODULE&&_ConfData[_i]->Conf[_j].info!=NULL;_j++){
                ModuleHTML_PAIR(_ret,_ConfData[_i]->Conf[_j].name,_ConfData[_i]->Conf[_j].info);
            }
            _ret<< "</table></div>" <<std::endl;
        }
        if(_src!=NULL){
            _ret<< "<div class=\"sub-title\"> Abxhttpd Variables </div>"<<std::endl;
            _ret<< "<div class=\"all-w\"><table class=\"main\">"<<std::endl;
            for(auto _i=_src->variables().begin(); _i!=_src->variables().end();++_i){
                ModuleHTML_PAIR(_ret,_i->first,_i->second);
            }
            _ret<< "</table></div>" <<std::endl;
            _ret<<std::endl;
        }
        return _ret.str();
    }

    Module::Module(ModuleConfigure * _info)
    {
        RegisterModule(_info);
    }
    
    Module::~Module(){}
}
