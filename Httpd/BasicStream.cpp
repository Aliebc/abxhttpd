#include "include/BasicStream.hxx"
#include <string.h>

namespace abxhttpd{

void BasicStream::clear_tmp(){
    memset(buffer_tmp, 0, ABXHTTPD_BASICSTREAM_BUFFER);
}

BasicStream::BasicStream(){
    buffer_tmp = new char [ABXHTTPD_BASICSTREAM_BUFFER];
    clear_tmp();
    status_id=0;
    err_str="success";
}

BasicStream::~BasicStream(){
    delete[] buffer_tmp;
}

bool BasicStream::open(void * arg){
    return true;
}

bool BasicStream::close(){
    return true;
}

size_t BasicStream::read(std::string & _dst,size_t size){
    return 0;
}

size_t BasicStream::write(const std::string & _content,size_t size){
    return 0;
}

int BasicStream::status() const{
    return status_id;
}

const char * BasicStream::GetLastError() const{
    return err_str;
}

BasicStream & operator>> ( BasicStream & src, std::string & is){
    src.read(is);
    return src;
}
BasicStream & operator<< (BasicStream & src, std::string & in){
    src.write(in);
    return src;
}
}
