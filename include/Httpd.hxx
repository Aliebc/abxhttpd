#ifndef HTTPD_H
#define HTTPD_H

#include "Platform.hxx"
#include "HttpRequest.hxx"
#include "HttpResponse.hxx"
#include "Module.hxx"

#ifdef ABXHTTPD_UNIX
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define F_SEP '/'
#endif

#ifdef ABXHTTPD_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>

#define F_SEP '\\'
#endif

#include <map>
#include <string>
#include <fstream>
#include <ostream>


#define ABXHTTPD_COREINITFUNC ABXHTTPD_MODINITFUNC

#define ABXHTTPD_CORE_MAX 64
extern int verbose;
extern int info_color;

#define ABXHTTPD_INFO_PRINT(grade,fmt,...) if(grade<=verbose){\
if(info_color==0){\
    printf("[Abxhttpd INFO][Grade %d] " fmt "\n",grade,##__VA_ARGS__);\
}else{printf("\033[1;35m[Abxhttpd INFO]\033[1;32m[Grade %d] \033[1;37m" fmt "\033[0m\n",grade,##__VA_ARGS__);fflush(stdout);}\
}

int HttpdInfo(int grade, const char * fmt, ...);

namespace abxhttpd{

typedef struct {
    bool Is_unix;
    bool Is_windows;
    int Port;
    int Bind_IP;
    long Bind_IP6;
    int Socket_modal;
    int allocated_socket;
    int Max_connect_count;
    bool Is_reused;
    void * Args;
}SocketSettingList;

typedef struct {
    size_t Stack_size;
    int Thread_count;
    int Socket_n;
    bool Multi_thread;
    bool Is_block;
    std::ostream * abxout;
    std::ostream * abxerr;
}ThreadSettingList;

typedef struct {
    std::string Path;
    void * Args;
}HttpSettingList;

typedef struct {
    SocketSettingList Socket_S;
    ThreadSettingList Thread_S;
    HttpSettingList Http_S;
} HttpdSettingList;

typedef HttpRequest (* IStreamFilter) (std::string &, void *);
typedef HttpResponse (* HttpHandler) (HttpRequest &, void *);
typedef std::string (* OStreamFilter) (HttpResponse &, void *);
typedef int (* SocketInitializer) (SocketSettingList &);

typedef struct {
    IStreamFilter IFilter;
    OStreamFilter OFilter;
    HttpHandler Handler;
} CCore;

using std::ostream;

typedef struct 
{
    int _ad;
    int _sd;
    struct sockaddr_in src_in;
    int port_in;
    CCore MCore;
    HttpSettingList Http_S;
} SocketRequest;

typedef void * (* ThreadController) (const ThreadSettingList &, const CCore &, void *);

extern HttpHandler DefaultHttpH;
extern SocketInitializer DefaultSocketI;
extern ThreadController DefaultThreadC;
extern IStreamFilter DefaultIFilter;
extern OStreamFilter DefaultOFilter;

typedef struct {
    IStreamFilter IFilter;
    OStreamFilter OFilter;
    SocketInitializer Initializer;
    ThreadController Controller;
    HttpHandler Handler;
} HttpdCore;

typedef struct {
    int _sd;
    int * _ad_list;
} _HttpdStatus;

extern HttpdCore DefaultHttpdCore;
extern _HttpdStatus HttpdSatatus;

typedef struct {
    char Symbol[32];
    char Info[128];
    HttpdCore *Core;
} HttpdCoreAddress;

extern HttpdCoreAddress DefaultHttpdCoreAddress;
extern HttpdCoreAddress HttpdCoreAddressTable[64];
typedef unsigned short int core_t;
extern core_t HttpdCoreAddressCount;
std::string ShowHttpdCoreAddressTable(void);
HttpdCore * FindHttpdCore(const char * _src);
int RegisterHttpdCore(HttpdCoreAddress * _core);

typedef int8_t httpd_t;

class Httpd
{
private:
    HttpdCore Core;
    CCore MCore;
    HttpdSettingList Setting;
    httpd_t _status;
public:
    Httpd(HttpdCore & _core, HttpdSettingList & _set);
    HttpdCore & core(void);
    httpd_t start(void);
    httpd_t start(void * _arg);
    httpd_t status() const;
    httpd_t stop();
    httpd_t stop(int _signal);
    ~Httpd();
};

}

#endif