#ifndef BASICSTREAM_H
#define BASICSTREAM_H
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

#define ABXHTTPD_BASICSTREAM_BUFFER 1024

#define ABXHTTPD_STREAM_CLOSED 0
#define ABXHTTPD_STREAM_READABLE (1<<0)
#define ABXHTTPD_STREAM_WRITEABLE (1<<1)

namespace abxhttpd{

class StreamBuffer:public std::stringbuf{
public:
    StreamBuffer(){

    }
};

class BasicStream{
    friend StreamBuffer;
private:
    const size_t __buffer_size=ABXHTTPD_BASICSTREAM_BUFFER;
protected:
    int status_id;
    char * buffer_tmp;
    void clear_tmp();
    const char * err_str;
    size_t buffer_size() const;
public:
    BasicStream();
    virtual bool open(void *);
    virtual bool close();
    virtual size_t read(std::string & _dst,size_t size = 0);
    virtual size_t write(const std::string & _content,size_t size = 0);
    virtual ~BasicStream();
    int status() const;
    const char * GetLastError() const;
    size_t transport(BasicStream & _dst,size_t size = 0);
    friend BasicStream& operator<< (BasicStream & src, std::string & in);
    friend BasicStream& operator>> (BasicStream & src, std::string & is);
    friend BasicStream& operator<< (BasicStream & to, BasicStream & from);
    friend BasicStream& operator>> (BasicStream & from, BasicStream & to);
};

class BasicFilter{
    
};

}
#endif
