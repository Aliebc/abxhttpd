#include "include/HttpResponse.hxx"
#include "include/HttpCode.hxx"
#include <stdio.h>
using namespace abxhttpd;

HttpResponse::HttpResponse(std::string & _src){
    this->Body=_src;
    this->Code=200;
    this->Protocol=std::string(ABXHTTPD_DEFAULT_PROTOCOL);
}

HttpResponse::HttpResponse(const char * _src,size_t _len){
    this->Body=std::string(_src,_len);
    this->Code=200;
    this->Protocol=std::string(ABXHTTPD_DEFAULT_PROTOCOL);
}

HttpResponse::HttpResponse(void){
    this->Body=std::string();
    this->Code=200;
    this->Protocol=std::string(ABXHTTPD_DEFAULT_PROTOCOL);
}

HttpResponse::HttpResponse(const char * _src){
    this->Body=std::string(_src);
    this->Code=200;
    this->Protocol=std::string(ABXHTTPD_DEFAULT_PROTOCOL);
}

std::string & HttpResponse::header(const std::string & _h){
    return this->Headers[_h];
}

std::string & HttpResponse::body(void){
    return this->Body;
}

std::string & HttpResponse::header(const char * _h){
    return this->header(std::string(_h));
}

std::string HttpResponse::raw(void){
    this->Raw+=this->Protocol+" ";
    this->Raw+=std::to_string(this->Code);
    this->Raw+=" ";
    this->Raw+=this->CodeInfo+"\r\n";
    StrArray::iterator _hi=this->Headers.begin();
    while(_hi!=this->Headers.end()){
        this->Raw+=_hi->first+":"+_hi->second+"\r\n";
        _hi++;
    }
    this->Raw+="\r\n";
    this->Raw+=this->Body;
    std::string _r=this->Raw;
    return _r;
}

bool HttpResponse::status(int _code,const std::string & _str){
    this->Code=_code;
    this->CodeInfo=_str;
    return true;
}

bool HttpResponse::status(int _code){
    this->Code=_code;
    this->CodeInfo=HttpCodeStatus(_code);
    return true;
}

HttpResponse::~HttpResponse(){

}