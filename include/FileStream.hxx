#ifndef FILE_SOCKET_H
#define FILE_SOCKET_H

#include <stdio.h>
#include "HttpdSocket.hxx"

namespace abxhttpd {
class FileStream:public BasicStream{
private:
    FILE * fp;
    void get_file_length();
    size_t length;
public:
    FileStream(const char * path);
    size_t read(std::string & dst,size_t size=0) override;
    size_t write(const std::string & dst,size_t size=0) override;
    size_t get_length() const;
    ~FileStream();
};
}

#endif
