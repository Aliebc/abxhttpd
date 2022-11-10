#include "include/Logger.hxx"
#include <ctime>

namespace abxhttpd{
    
Logger::Logger(const char *path)
:FileStream(path,FILE_FLAG::ADD|FILE_FLAG::BINARY){}

Logger::Logger(int fd):FileStream(fd){
    if(status_id^BasicStream::FLAG::WRITEABLE){
        throw abxhttpd_error("Cannot write to logger target");
    }
}

void Logger::stfr_time(){
    get_time();
    clear_tmp();
    strftime(buffer_tmp, 128, "[%Y-%m-%d %H:%M:%S] ", localtime(&now));
}

void Logger::get_time(){
    time(&now);
}

size_t Logger::read(std::string &dst, size_t size){
    return 0;
}

size_t Logger::write(const std::string &dst, size_t size){
    size_t _write_len=0;
    {
        std::unique_lock<std::mutex> iolock(log_lock);
        stfr_time();
        std::string __time(buffer_tmp);
        _write_len+=this->FileStream::write(__time);
        _write_len+=this->FileStream::write(dst, size);
        _write_len+=this->FileStream::write("\n");
    }
    return _write_len;
}

Logger::~Logger()=default;

}
