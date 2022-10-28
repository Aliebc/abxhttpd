#ifndef _SELECT_H
#define _SELECT_H

#include "abxhttpd.H"
#include "HttpdThreadPool.hxx"
#include <vector>
#include <map>

namespace abxhttpd{

class HttpdPoll{
private:
    int MSocketID;
    int npfd;
    int SLsize;  //memory of SocketList
    struct pollfd * SocketList;
    bool running;
    const CCore * _core;
    const ThreadSettingList * thread_s;
    HttpdSettingList http_s;
    std::map<int,SocketRequestWithSL *> SocketMap;
    int type;
    const char * last_err;
    bool accept();
    bool alloc_poll();
public:
    HttpdPoll(const ThreadSettingList& _set, const CCore& _core,const HttpdSettingList * http_s,int StreamType=ABXHTTPD_SOCK_STREAM);
    ~HttpdPoll();
    bool run();
    bool shutdown();
    const char * GetLastError();
};

}
#endif
