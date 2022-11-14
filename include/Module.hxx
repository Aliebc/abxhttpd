#ifndef MODULE_H
#define MODULE_H
#include "Version.hxx"
#include "HttpRequest.hxx"
#include <cstring>
#include <string>

#define ABXHTTPD_MAX_MODULE 64

namespace abxhttpd{

    typedef struct {
        const char * Name;
        struct {
            const char * name;
            const char * info;
        } Conf[ABXHTTPD_MAX_MODULE];
        void *(*dfunc)(void *);
        void * data;
    }ModuleConfigure;
    
    typedef unsigned int module_t;

    class ABXHTTPD_API Module
    {
    private:
        static ModuleConfigure * _ConfData[ABXHTTPD_MAX_MODULE];
        static module_t ModuleCount;
        void RegisterModule(ModuleConfigure * _info);
    public:
        static std::string ShowModules(char sep='\n');
        static std::string ShowModules_HTML(const HttpRequest * _src=NULL);
        static void init();
        explicit Module(ModuleConfigure * _info);
        ~Module();
    };
}

#ifdef ABXHTTPD_GUI
namespace abxhttpd{
    int start_gui(void * _args);
}
#endif

#endif
