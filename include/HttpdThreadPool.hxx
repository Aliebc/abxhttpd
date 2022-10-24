#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include "abxhttpd.H"

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <chrono>

namespace abxhttpd{

class HttpdThreadPool{
private:
    int thread_count;
    std::thread * thread_list;
    std::mutex thread_lock;
    std::condition_variable thread_cv;
    std::queue<void *> data_queue;
    int idle_thread;
    bool is_running;
    void *(*data_func)(void *);
    int task_count;
    void _MainThread();
public:
    HttpdThreadPool(int count, void *(*d_func)(void *));
    bool push(void *);
    void stop();
    ~HttpdThreadPool();
};
}

#endif
