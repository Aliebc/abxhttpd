#include "include/HttpdThreadPool.hxx"

namespace abxhttpd{

HttpdThreadPool::HttpdThreadPool(int count, void *(*d_func)(void *)){
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
                        thread_cv.wait_for(tlck,std::chrono::milliseconds(500));
                        if(!data_queue.empty()){
                            _ptr=data_queue.front();
                            data_queue.pop();
                        }else{
                            continue;
                        }
                    }
                    data_func(_ptr);
                    {
                        std::unique_lock<std::mutex> tlck(thread_lock);
                        idle_thread++;
                    }
                }
                {
                    std::unique_lock<std::mutex> tl(internal_lock);
                    std::cout << "Thread " <<std::this_thread::get_id() <<" Exit\n";
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
        if(idle_thread>0){
            {
                std::unique_lock<std::mutex> tlck(thread_lock);
                data_queue.push(_ptr);
                idle_thread--;
                thread_cv.notify_one();
            }
            return true;
        }else{
            return false;
        }
    }
    return true;
}

void HttpdThreadPool::stop(){
    is_running=false;
    thread_cv.notify_all();
    delete [] thread_list;
}

}
