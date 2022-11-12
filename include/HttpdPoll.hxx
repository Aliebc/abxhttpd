#ifndef _SELECT_H
#define _SELECT_H

#include "HttpdThreadPool.hxx"
#include <vector>
#include <map>

namespace abxhttpd{

/**
 * @brief IO多路复用控制类
 *
 * 本类用于控制IO多路复用，其核心在于通过Poll进行套接字复用，内部维护了一个套接字监听表和流对应表，
 * 在析构的时候会自动释放
 */
class HttpdPoll{
private:
    int MSocketID; ///< 核心套接字描述符
    int npfd; ///< 需要监听的描述符数量
    int SLsize;  ///< 内存中能放下的描述符数量（会动态扩增）
    struct pollfd * SocketList; ///< 需要监听的描述符
    bool running; ///< 是否争做运行
    const CCore * _core; ///< 传入的控制核心
    const ThreadSettingList * thread_s; ///< 传入的线程设置
    HttpdSettingList http_s; ///< 传入的服务器设置
    std::map<int,SocketRequestWithSL *> SocketMap; ///< 需要监听的描述符和控制流的对应
    int type; ///< 套接字流类型
    const char * last_err; ///< 指向错误信息
    bool accept(); ///< 接受新的套接字
    bool alloc_poll(); ///< 为新的套接字分配空间
public:
    ///构造函数
    HttpdPoll(const ThreadSettingList& _set, const CCore& _core,const HttpdSettingList * http_s,int StreamType=ABXHTTPD_SOCK_STREAM);
    ///析构函数
    ~HttpdPoll();
    /// 公有的可以用来接收新套接字的函数（为其他模块准备）
    static int accept(int sd,SocketRequestWithSL *,int type=ABXHTTPD_SOCK_STREAM);
    /// 开始运行
    bool run();
    /// 结束运行
    bool shutdown();
    /// 返回错误
    const char * GetLastError() noexcept;
};

}
#endif
