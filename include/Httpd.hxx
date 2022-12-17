#ifndef HTTPD_H
#define HTTPD_H

#include "Platform.hxx"
#include "BasicHttp.hxx"
#include "HttpRequest.hxx"
#include "HttpResponse.hxx"
#include "Module.hxx"
#include "Logger.hxx"
#include "config.hxx"

#include <string>
#include <fstream>
#include <ostream>

#define ABXHTTPD_CORE_MAX 64 ///< 最大核心数量
#define ABXHTTPD_CONNECT_MAX SOMAXCONN ///< 最大队列数量(可等待多少套接字排队)


/**
 * @brief 核心命名空间
 * 
 * 所有的类与函数都在这个命名空间下, 通过using namespace abxhttpd去调用
 */
namespace abxhttpd{

/**
 * @brief 套接字设置列表
 * 
 */
typedef struct {
    int IPVer; ///< IP版本
    const char * IPStr; ///< 字符串形式的IP
    int Port; ///< 端口(不要进行大小端转换)
    int MaxConnect;/// <最大队列数量,推荐设置为ABXHTTPD_CONNECT_MAX
    int allocated_socket; ///< 分配到的套接字描述符
    void * Args; ///< 其他可携带数据
}SocketSettingList;

/**
 * @brief 线程控制器设置列表
 * 
 */
typedef struct _ThreadSettingList {
    int ThreadCount;
    int SocketMainID;
    bool Multithreading;
    bool Running;
    const char * Log;
    const char * Error;
    void * Args; ///< 其他可携带数据
} ThreadSettingList;

/**
 * @brief Http设置列表
 * 
 */
typedef struct {
    const char * Path; ///< 路径
    void * Args; ///< 其他可携带数据
}HttpSettingList;

/// Http服务器设置列表
typedef struct {
    SocketSettingList Socket_S; ///< 套接字设置列表
    ThreadSettingList Thread_S; ///< 线程控制器设置列表
    HttpSettingList Http_S; ///< Http设置列表
} HttpdSettingList;

/// 套接字构造器函数
typedef int (* SocketInitializer) (SocketSettingList &);
/// 套接字析构器函数
typedef int (* SocketDestructor) (const SocketSettingList &);

/// 控制流核心(用户不应该直接处理这个核心)
typedef struct {
    IStreamFilter IFilter; ///< 输入流过滤器 (未来将弃用,此为兼容设置)
    OStreamFilter OFilter; ///< 输出流过滤器 (未来将弃用,此为兼容设置)
    HttpHandler Handler; ///< 核心http处理器
} CCore;

using std::ostream;

/**
 * @brief 套接字（TCP）请求
 *
 * 此结构体代表一次有效的TCP请求
 */
typedef struct 
{
    int _ad; ///< 子套接字描述符
    int _sd; ///< 监听套接字描述符
    struct sockaddr_in src_in; ///< 未转换的源IP
    std::string src_in_ip; ///< 经转换的的源IP
    int port_in; ///< 源端口
    const CCore * MCore; ///< 核心处理
    HttpSettingList Http_S; ///< Httpd设置
} SocketRequest;

/// 线程控制器函数
typedef void * (* ThreadController) (const ThreadSettingList &, const CCore &, void *);

ABXHTTPD_API extern HttpHandler DefaultHttpH; ///< 默认核心http处理器
ABXHTTPD_API extern SocketInitializer DefaultSocketI; ///< 默认套接字构造器
ABXHTTPD_API extern SocketDestructor DefaultSocketD; ///< 默认套接字析构器
ABXHTTPD_API extern ThreadController DefaultThreadC; ///< 默认核心控制器
ABXHTTPD_API extern IStreamFilter DefaultIFilter; ///< 默认输入流过滤器 (未来将弃用,此为兼容设置)
ABXHTTPD_API extern OStreamFilter DefaultOFilter; ///< 默认输出流过滤器 (未来将弃用,此为兼容设置)

/**
 * @brief Http核心结构体
 * 
 * Httpd核心结构体由输入流过滤器、输出流过滤器、套接字构造器、套接字析构器、核心控制器、核心http处理器构成
 * 
 * 
 */
typedef struct {
    IStreamFilter IFilter; ///< 输入流过滤器 (未来将弃用,此为兼容设置)
    OStreamFilter OFilter; ///< 输出流过滤器 (未来将弃用,此为兼容设置)
    SocketInitializer Initializer; ///< 套接字构造器
    SocketDestructor Destructor; ///< 套接字析构器
    ThreadController Controller; ///< 核心控制器
    HttpHandler Handler;///< 核心http处理器
} HttpdCore;

ABXHTTPD_API extern HttpdCore DefaultHttpdCore; ///<默认核心,如果用户需要自定义核心http核心,建议从这里复制

/// 核心地址
typedef struct {
    const char * Symbol; ///< 文字标识符
    const char * Info; ///< 基本信息
    const HttpdCore *Core; ///< 核心地址
} HttpdCoreAddress;

typedef unsigned short int core_t;
extern core_t HttpdCoreAddressCount;
ABXHTTPD_API std::string ShowHttpdCoreAddressTable(char sep='\n');
ABXHTTPD_API const HttpdCore * FindHttpdCore(const char * _src);
void RegisterHttpdCore(HttpdCoreAddress _core);

typedef int8_t httpd_t; ///< httpd状态码

/**
 * @brief 核心Http服务器类 
 * 
 * Abxhttpd的核心类,由此构建一个http服务器,调用start方法启动
 * 
 */
class ABXHTTPD_API Httpd
{
private:
    HttpdCore Core; ///< 服务器核心
    CCore MCore; ///< 控制流核心
    HttpdSettingList Setting; ///< 内部存储的服务器设置列表
    httpd_t _status; ///< 服务器当前状态
    static void env(); ///< 准备环境
    static SSMap ExtraSettingList; //< 内部存储的拓展设置列表
public:
    Httpd(const HttpdCore & _core,const HttpdSettingList & _set);
    httpd_t start(); ///< 启动服务器
    httpd_t status() const; //< 获取服务器状态
    httpd_t stop(); ///< 关闭服务器
    httpd_t stop(int _signal); ///< 未来实现
    /**
     * @brief 获取拓展设置(给拓展模块使用的)
     * 
     * @param name 设置的名称
     * @return const char* 
     */
    static const char * GetExtraSetting(const char * name);
    /**
     * @brief 传入拓展设置(给拓展模块使用的)
     * 
     * @param name 设置的名称
     * @param val 设置的值
     */
    static void SetExtraSetting(const char * name, const char * val); ///<获取其他设置
    static Logger * success_logger;
    static Logger * except_logger;
    /**
     * @brief 信息等级
     * @param 大于0的整数 信息的等级
     */
    static int verbose;
    /**
     * @brief 颜色指示符
     * @param 大于0的整数 输出颜色
     * @param 小于0的整数 不输出颜色
     */
    static int info_color; ///< 
    static int global_argc; ///< 全局参数个数
    static const char ** global_argv; ///< 全局参数列表
    static const CmdArray * global_argu; ///< 全局参数列表（解析后）
    ~Httpd();
};

/// 核心地址构造类
class HttpdCore_R 
{
public:
    explicit HttpdCore_R(const HttpdCoreAddress &);
    ~HttpdCore_R();
};

}

#endif
