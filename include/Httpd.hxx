#ifndef HTTPD_H
#define HTTPD_H

#include "Platform.hxx"
#include "BasicHttp.hxx"
#include "HttpRequest.hxx"
#include "HttpResponse.hxx"
#include "Module.hxx"
#include "Logger.hxx"

#include <string>
#include <fstream>
#include <ostream>

#define ABXHTTPD_CORE_MAX 64
#define ABXHTTPD_CONNECT_MAX SOMAXCONN

namespace abxhttpd{

typedef struct {
    int IPVer;
    const char * IPStr;
    int Port;
    int MaxConnect;
    int allocated_socket;
    void * Args;
}SocketSettingList;


typedef struct _ThreadSettingList {
    int ThreadCount;
    int SocketMainID;
    bool Multithreading;
    bool Running;
    const char * Log;
    const char * Error;
    void * Args;
} ThreadSettingList;

typedef struct {
    const char * Path;
    void * Args;
}HttpSettingList;

typedef struct {
    SocketSettingList Socket_S;
    ThreadSettingList Thread_S;
    HttpSettingList Http_S;
} HttpdSettingList;

typedef int (* SocketInitializer) (SocketSettingList &);
typedef int (* SocketDestructor) (const SocketSettingList &);


typedef struct {
    IStreamFilter IFilter;
    OStreamFilter OFilter;
    HttpHandler Handler;
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

typedef void * (* ThreadController) (const ThreadSettingList &, const CCore &, void *);

ABXHTTPD_API extern HttpHandler DefaultHttpH;
ABXHTTPD_API extern SocketInitializer DefaultSocketI;
ABXHTTPD_API extern SocketDestructor DefaultSocketD;
ABXHTTPD_API extern ThreadController DefaultThreadC;
ABXHTTPD_API extern IStreamFilter DefaultIFilter;
ABXHTTPD_API extern OStreamFilter DefaultOFilter;

typedef struct {
    IStreamFilter IFilter;
    OStreamFilter OFilter;
    SocketInitializer Initializer;
    SocketDestructor Destructor;
    ThreadController Controller;
    HttpHandler Handler;
} HttpdCore;

ABXHTTPD_API extern HttpdCore DefaultHttpdCore;

typedef struct {
    const char * Symbol;
    const char * Info;
    const HttpdCore *Core;
} HttpdCoreAddress;

typedef unsigned short int core_t;
extern core_t HttpdCoreAddressCount;
ABXHTTPD_API std::string ShowHttpdCoreAddressTable(char sep='\n');
ABXHTTPD_API const HttpdCore * FindHttpdCore(const char * _src);
void RegisterHttpdCore(HttpdCoreAddress _core);

typedef int8_t httpd_t;

class ABXHTTPD_API Httpd
{
private:
    HttpdCore Core;
    CCore MCore;
    HttpdSettingList Setting;
    httpd_t _status;
    static void env(); ///< Prepare environment
    static SSMap ExtraSettingList;
public:
    Httpd(const HttpdCore & _core,const HttpdSettingList & _set);
    httpd_t start();
    httpd_t start(void * _arg);
    httpd_t status() const;
    httpd_t stop();
    httpd_t stop(int _signal);
    static const char * GetExtraSetting(const char *);
    static void SetExtraSetting(const char *, const char *);
    static Logger * success_logger;
    static Logger * except_logger;
    static int verbose;
    static int info_color;
    ~Httpd();
};

class HttpdCore_R 
{
public:
    explicit HttpdCore_R(const HttpdCoreAddress &);
    ~HttpdCore_R();
};

}

#endif
