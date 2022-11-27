#include "include/BasicHttp.hxx"

namespace abxhttpd{
HttpParserException::HttpParserException(int sid):
except_id(sid){}

HttpParserException::HttpParserException(int sid,const char * msg)
:HttpParserException(sid)
{
    err_msg=msg;
}

const char * HttpParserException::what() const noexcept{
    return err_msg;
}

int HttpParserException::code() const{
    return except_id;
}

HttpParserException::~HttpParserException()=default;

BasicHttp::BasicHttp()=default;

BasicHttp::~BasicHttp()=default;

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

size_t BasicHttp::length() const{
    return Length;
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
}
