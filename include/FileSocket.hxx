#ifndef FILE_SOCKET_H
#define FILE_SOCKET_H

#include <stdio.h>
#include "HttpdSocket.hxx"

namespace abxhttpd {
class FileSocket:public HttpdSocket{
private:
    FILE * fp;
    void get_file_length();
    size_t length;
    char tmp[ABXHTTPD_BUFFER_SIZE];
public:
    FileSocket(const char * path);
    size_t read(std::string & dst,size_t size=0);
    size_t write(const std::string & dst,size_t size=0);
    size_t get_length();
    ~FileSocket();
    friend HttpdSocket & operator<< (HttpdSocket & , FileSocket &);
};
}

#endif
