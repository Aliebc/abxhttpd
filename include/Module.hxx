#ifndef MODULE_H
#define MODULE_H
#include "Version.hxx"
#include "HttpRequest.hxx"
#include <cstring>
#include <string>

#define ABXHTTPD_MAX_MODULE 64

namespace abxhttpd{
    
    /**
     * @brief 模块基本信息
     *  
     *  信息会展示在HTML默认信息页面上
     * 
     */
    typedef struct {
        const char * Name; ///< 模块名
        struct {
            /// 键
            const char * name;
            /// 值
            const char * info;
        } Conf[ABXHTTPD_MAX_MODULE];
        /// 回调函数
        void *(*dfunc)(void *);
        /// 回调函数数据
        void * data;
    }ModuleConfigure;
    
    typedef unsigned int module_t;
    /**
     * @brief 拓展模块类
     * 
     */
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
        /**
         * @brief 注册模块
         * 
         * @param _info 通过 @link ModuleConfigure @endlink 进行模块注册
         */
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
