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
    }ConfigureInfo;
    
    typedef unsigned int module_t;

    class ABXHTTPD_API Module
    {
    private:
        static ConfigureInfo * _ConfData[ABXHTTPD_MAX_MODULE];
        static module_t ModuleCount;
        void RegisterModule(ConfigureInfo * _info);
    public:
        static std::string ShowModules(char sep='\n');
        static std::string ShowModules_HTML(HttpRequest * _src=NULL);
        explicit Module(ConfigureInfo * _info);
        Module(ConfigureInfo * _info, void *(*dfunc)(void *), void * dta);
        ~Module();
    };
}

#ifdef ABXHTTPD_GUI
namespace abxhttpd{
    int start_gui(void * _args);
}
#endif

#endif
