#include "include/HttpdThreadPool.hxx"

namespace abxhttpd{

HttpdThreadPool::HttpdThreadPool(int count, void *(*d_func)(void *)):
task_count(0){
    if(count>0){
        is_running=true;
        thread_count=count;
        idle_thread=thread_count;
        data_func=d_func;
        thread_list=new std::thread[thread_count];
        for(int _i=0;_i<thread_count;_i++){
            std::thread tmp([&](){
                while(is_running){
                    void * _ptr;
                    {
                        std::unique_lock<std::mutex> tlck(thread_lock);
                        while(data_queue.empty()){
                            thread_cv.wait_for(tlck,std::chrono::milliseconds(500));
                        }
                        _ptr=data_queue.front();
                        data_queue.pop();
                    }
                    void * rptr=data_func(_ptr);
                    if(rptr!=NULL){
                        push(rptr);
                    }
                    {
                        std::lock_guard<std::mutex> tlck(thread_lock);
                        idle_thread++;
                    }
                }
            });
            thread_list[_i]=std::move(tmp);
        }
    }
}
HttpdThreadPool::~HttpdThreadPool(){
    if(is_running){
        stop();
    }
}

bool HttpdThreadPool::push(void * _ptr){
    {
        if(true){
            {
                std::lock_guard<std::mutex> tlck(thread_lock);
                data_queue.push(_ptr);
                idle_thread--;
                thread_cv.notify_one();
            }
            return true;
        }else{
            return false;
        }
    }
}

void HttpdThreadPool::stop(){
    is_running=false;
    thread_cv.notify_all();
    //delete [] thread_list;
}

}
