#include "include/BasicHttp.hxx"

namespace abxhttpd{
BasicHttpException::BasicHttpException(int sid):
except_id(sid){}

BasicHttpException::BasicHttpException(int sid,const char * msg)
{
    err_msg=msg;
}

const char * BasicHttpException::what() const noexcept{
    return err_msg;
}

int BasicHttpException::code() const{
    return except_id;
}

BasicHttp::BasicHttp(){
    
}

BasicHttp::~BasicHttp(){

}

void BasicHttp::append(const std::string &source){
    Buffer.append(source);
}

bool BasicHttp::is_header(const std::string & _h) const{
    return !(headers().find(_h)==headers().end());
}

const std::string & BasicHttp::header(const std::string & _h) const{
    if(is_header(_h)){
        return headers().at(_h);
    }else{
        return null_str;
    }
}

void BasicHttp::header(const std::string & _h,const std::string && _v){
    (Headers)[_h]=_v;
}

void BasicHttp::header(const std::string & _h,const std::string & _v){
    (Headers)[_h]=_v;
}

int BasicHttp::status() const{
    return status_id;
}

const HttpHeaders & BasicHttp::headers() const{
    return Headers;
}

BasicHttpFilter::BasicHttpFilter(BasicStream & src, BasicStream & dst)
:BasicFilter(src, dst){}

size_t BasicHttpFilter::StreamFilter(BasicStream & f, BasicStream & t, size_t size){
    return 0;
}


}
