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

/**
 * @brief 日志流
 *
 * 日志流，实现线程安全的日志系统
 */
class Logger:public FileStream{
private:
    time_t now; ///< 当前时间
    void get_time(); ///< 获取当前时间
    void stfr_time(); ///< 格式化为本地时间
    std::mutex log_lock; ///< IO锁
public:
    explicit Logger(const char *); ///< 路径初始化
    explicit Logger(int fd); ///< 描述符初始化
    size_t read(std::string &dst, size_t size = 0) override;
    size_t write(const std::string &dst, size_t size = 0) override;
    ~Logger();
};

}

#endif
