#ifndef LOGGER_HXX
#define LOGGER_HXX

#include "BasicStream.hxx"
#include "FileStream.hxx"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>
#include <ctime>
#include <sstream>

namespace abxhttpd{

class Logger:public FileStream{
private:
    time_t now;
    void get_time();
    void stfr_time();
    std::mutex log_lock;
    int buffer_size;
public:
    Logger(const char *);
    Logger(int fd);
    size_t read(std::string &dst, size_t size = 0) override;
    size_t write(const std::string &dst, size_t size = 0) override;
    ~Logger();
    
};

}

#endif
