#ifndef FILE_SOCKET_H
#define FILE_SOCKET_H

#include <stdio.h>
#include "HttpdSocket.hxx"

#define ABXHTTPD_FILE_READ 1<<0
#define ABXHTTPD_FILE_WRITE 1<<1
#define ABXHTTPD_FILE_BINARY 1<<2
#define ABXHTTPD_FILE_ADD (1<<3|ABXHTTPD_FILE_WRITE)

#define ABXHTTPD_FILE_STDIN 0
#define ABXHTTPD_FILE_STDOUT 1
#define ABXHTTPD_FILE_STDERR 2

namespace abxhttpd {
class FileStream:public BasicStream{
private:
    FILE * fp;
    void get_file_length();
    size_t length;
protected:
    static const char * open_method[];
public:
    FileStream();
    FileStream(const char * path);
    FileStream(int fd);
    FileStream(const char * path,int method);
    size_t read(std::string & dst,size_t size=0) override;
    size_t write(const std::string & dst,size_t size=0) override;
    size_t get_length() const;
    ~FileStream();
};
}

#endif
