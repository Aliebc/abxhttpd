#include "include/Module.hxx"

namespace abxhttpd{
    _Module ModuleList[128];
    module_t ModuleCount=0;

    void RegisterModule(const char * _name,const char * _info){
        _Module _mod;
        strcpy(_mod.Name,_name);
        strcpy(_mod.Info,_info);
        memcpy((void *)&ModuleList[ModuleCount],(void *)&_mod,sizeof(_Module));
        ModuleCount++;
    }

    std::string ShowModules(){
        std::string _ret;
        for(module_t _i=0;_i<ModuleCount;_i++){
            _ret+=std::string(ModuleList[_i].Name)+"\t"+std::string(ModuleList[_i].Info)+"\n";
        }
        return _ret;
    }

    Module::Module(const char * _name,const char * _info)
    {
        RegisterModule(_name,_info);
    }
    
    Module::~Module()
    {
    }
}