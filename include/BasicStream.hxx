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
#define ABXHTTPD_STREAM_EXCEPT (1<<2)

namespace abxhttpd{

class StreamBuffer:public std::stringbuf{
public:
    StreamBuffer(){

    }
};

class BasicStream{
    friend StreamBuffer;
private:
    static const size_t __buffer_size=ABXHTTPD_BASICSTREAM_BUFFER;
    //std::stringstream internal_tmp;
protected:
    int status_id;
    char * buffer_tmp;
    void clear_tmp();
    const char * err_str;
    void SetLastError(const char *);
public:
    BasicStream();
    static size_t buffer_size();
    virtual bool open(void *);
    virtual bool close();
    virtual size_t read(std::string & _dst,size_t size = 0);
    //virtual size_t read(BasicStream & _dst,size_t size = 0);
    virtual size_t write(const std::string & _content,size_t size = 0);
    //virtual size_t write(BasicStream & _content,size_t size = 0);
    virtual ~BasicStream();
    int status() const;
    const char * GetLastError();
    size_t transport(BasicStream & _dst,size_t size = 0);
    friend BasicStream& operator<< (BasicStream & src, std::string & in);
    friend BasicStream& operator<< (BasicStream & src, const char * in);
    friend BasicStream& operator>> (BasicStream & src, std::string & is);
    friend BasicStream& operator<< (BasicStream & to, BasicStream & from);
    friend BasicStream& operator>> (BasicStream & from, BasicStream & to);
};

typedef size_t(*BasicStringFilter)(BasicStream &,BasicStream &);

class BasicFilter{
private:
    static size_t DefaultFilter(std::string &,std::string &);
protected:
    int status_id;
    const char * err_msg;
    BasicStream & src;
    BasicStream & dst;
    BasicStringFilter i_filter;
    std::string tmp;
public:
    BasicFilter(BasicStream &,BasicStream &,BasicStringFilter);
    void from(BasicStream &);
    void to(BasicStream &);
    void filter(BasicStringFilter);
    virtual ~BasicFilter();
    virtual size_t exec(size_t);
    int status(int) const;
    const char * GetLastError() const;
};

}
#endif
