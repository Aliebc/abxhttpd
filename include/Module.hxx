#ifndef MODULE_H
#define MODULE_H
#include "Version.hxx"
#include <cstring>
#include <string>

namespace abxhttpd{
    typedef struct 
    {
        char Name[64];
        char Info[128];
    } Module;
    typedef unsigned int module_t;
    extern Module ModuleList[128];
    extern module_t ModuleCount;
    void RegisterModule(const char * _name,const char * _info);
    std::string ShowModules();
}

#ifndef ABXHTTPD_MSVC
#define ABXHTTPD_MODINITFUNC __attribute__((constructor)) void
#else
#define ABXHTTPD_MODINITFUNC void
#endif

#ifdef ABXHTTPD_GUI
namespace abxhttpd{
    int start_gui(void * _args);
}
#endif

#endif