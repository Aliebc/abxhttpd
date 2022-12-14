#include "include/BasicStream.hxx"
#include <string.h>

namespace abxhttpd{

void BasicStream::clear_tmp(){
    memset(buffer_tmp, 0, __buffer_size);
}

BasicStream::BasicStream(){
    buffer_tmp = new char [__buffer_size+1]();
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

size_t BasicStream::buffer_size(){
    return __buffer_size;
}

const char * BasicStream::GetLastError(){
    status_id^=EXCEPTION;
    const char * resp=err_str;
    err_str="success";
    return resp;
}

void BasicStream::SetLastError(const char * err){
    status_id|=EXCEPTION;
    err_str=err;
}

BasicStream & operator>> ( BasicStream & src, std::string & is){
    src.read(is);
    return src;
}

BasicStream & operator<< (BasicStream & src, const std::string & in){
    src.write(in);
    return src;
}

BasicStream & operator<< (BasicStream & src, const char * in){
    src.write(in);
    return src;
}

BasicStream & operator<< (BasicStream & to, BasicStream & from){
    std::string tmp_s;
    size_t _read_len=0;
    while ((_read_len=from.read(tmp_s,from.buffer_size()))!=0) {
        if(_read_len==0){
            break;
        }
        to.write(tmp_s,_read_len);
        tmp_s.clear();
        /*if(!(from.status()&BasicStream::FLAG::READABLE)||!(to.status()&BasicStream::FLAG::WRITEABLE)){
            break;
        }*/
    }
    return to;
}

BasicStream & operator>> (BasicStream & from, BasicStream & to){
    to<<from;
    return from;
}

BasicFilter::BasicFilter(BasicStream & src,BasicStream & dst):
source(src),destination(dst){
    status_id=B_FLAG::SUCCESS;
}

size_t BasicFilter::exec(size_t s){
    if(s==0){s--;}
    return StreamFilter(source, destination, s);
}

int BasicFilter::status(int which) const{
    int sid;
    switch (which) {
        case WHICH::FROM:
            sid=source.status();
            break;
        case WHICH::TO:
            sid=destination.status();
            break;
        default:
            sid=0;
            break;
    }
    
    return sid;
}

int BasicFilter::status() const{
    return status_id;
}

size_t BasicFilter::StreamFilter(BasicStream & f,BasicStream & t,size_t size){
    size_t r_size;
    tmp.clear();
    f.read(tmp,size);
    r_size=t.write(tmp,size);
    tmp.clear();
    tmp.shrink_to_fit();
    return r_size;
}

size_t BasicFilter::length() const{
    return handled_length;
}

const char * BasicFilter::GetLastError() const noexcept{
    return err_msg;
}

}