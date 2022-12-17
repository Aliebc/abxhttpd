#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include "HttpdSocket.hxx"
#include "BasicHttpFilter.hxx"

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <chrono>

namespace abxhttpd{

/**
 * @brief 带有详细信息的套接字请求类
 * 
 */
typedef struct _SocketRequestWithSL
{
    const ThreadSettingList * thread_s; ///< 线程设置信息
    HttpdSocket * socket_p; ///< 套接字输入输出流
    bool free;
    BasicHttpFilter * filter; ///< 过滤器类
    ~_SocketRequestWithSL(){
        delete socket_p;
        delete filter;
    }
}SocketRequestWithSL;

/**
 * @brief 线程池类
 * 
 * 初始化一些线程, 用线程池处理大批量的任务
 */
class HttpdThreadPool{
private:
    int thread_count; ///< 线程数量
    std::thread * thread_list; ///< 线程列表 
    std::mutex thread_lock; ///< 线程锁
    std::condition_variable thread_cv; ///< 条件变量
    std::queue<void *> data_queue; ///< 数据队列
    int idle_thread; ///< 空闲线程数量
    bool is_running; ///< 运行状态符
    void *(*data_func)(void *); ///< 数据处理函数
    int task_count; ///< 任务数量
public:
    /**
     * @brief 构造函数
     * 
     * @param count 初始化线程数量
     * @param d_func 数据处理函数
     */
    HttpdThreadPool(int count, void *(*d_func)(void *));
    /**
     * @brief 任务提交函数
     * 
     * @return true 提交成功
     * @return false 提交失败
     */
    bool push(void *);
    /**
     * @brief 终止线程
     * 
     */
    void stop();
    /**
     * @brief 析构函数,回收线程资源
     * 
     */
    ~HttpdThreadPool();
    HttpdThreadPool(const HttpdThreadPool &)=delete;
    void operator= (const HttpdThreadPool &)=delete;
};
}

#endif
