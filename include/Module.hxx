#ifndef MODULE_H
#define MODULE_H
#include "Version.hxx"
#include "HttpRequest.hxx"
#include <cstring>
#include <string>

#define ABXHTTPD_MAX_MODULE 64

namespace abxhttpd{

    typedef struct {
        char name[32];
        char info[64];
    } StrKey;
    typedef struct {
        char Name[64];
        StrKey Conf[64];
    }ConfigureInfo;
    
    typedef unsigned int module_t;
    extern ConfigureInfo _ConfData[ABXHTTPD_MAX_MODULE];
    extern module_t ModuleCount;

    void RegisterModule(ConfigureInfo _info);
    void RegisterModule(ConfigureInfo _info);
    std::string ShowModules();
    std::string ShowModules_HTML(HttpRequest * _src=NULL);

    class Module
    {
    private:
        
    public:
        Module(ConfigureInfo _info);
        ~Module();
    };
}

#ifdef ABXHTTPD_GUI
namespace abxhttpd{
    int start_gui(void * _args);
}
#endif

#endif