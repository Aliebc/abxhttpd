#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

#include <stdio.h>
#include <string>
#include <map>
#include "BasicStream.hxx"

namespace abxhttpd{
typedef std::map <std::string, std::string> HttpHeaders;

class BasicHttpException{
protected:
    int except_id;
    const char * err_msg;
public:
    BasicHttpException(int);
    BasicHttpException(int,const char *);
    ~BasicHttpException();
    int id() const;
    const char * what () const;
};

class BasicHttp:BasicFilter{
protected:
    size_t Length;
    HttpHeaders Headers;
    std::string Buffer;
public:
    size_t exec(size_t);
    std::string header() const;
    void header(const std::string &,const std::string &);
    //virtual size_t read();
    
};

}

#endif
