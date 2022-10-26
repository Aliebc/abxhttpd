#include "include/HttpResponse.hxx"
#include "include/HttpCode.hxx"
#include <stdio.h>
#include <iostream>
using namespace abxhttpd;

HttpResponse::HttpResponse(std::string & _src){
    Body=_src;
    Code=200;
    Protocol=ABXHTTPD_DEFAULT_PROTOCOL;
}

HttpResponse::HttpResponse(const char * _src,size_t _len){
    Body=std::string(_src,_len);
    Code=200;
    Protocol=ABXHTTPD_DEFAULT_PROTOCOL;
}

HttpResponse::HttpResponse(void){
    Body=std::string();
    Code=200;
    Protocol=ABXHTTPD_DEFAULT_PROTOCOL;
}

HttpResponse::HttpResponse(const char * _src){
    Body=std::string(_src);
    Code=200;
    Protocol=ABXHTTPD_DEFAULT_PROTOCOL;
}

std::string & HttpResponse::header(const std::string & _h){
    return Headers[_h];
}

std::string & HttpResponse::body(void){
    return Body;
}

void HttpResponse::body(const std::string &&_src){
    Body=std::move(_src);
}

std::string & HttpResponse::header(const char * _h){
    return header(std::string(_h));
}

void HttpResponse::header(const char * _h,std::string && _src){
    _src.insert(0L, " ");
    Headers[_h]=std::move(_src);
}

std::string HttpResponse::raw(void){
    Raw+=Protocol+" ";
    Raw+=std::to_string(Code);
    Raw+=" ";
    Raw+=CodeInfo+"\r\n";
    StrArray::iterator _hi=Headers.begin();
    while(_hi!=Headers.end()){
        Raw+=_hi->first+":"+_hi->second+"\r\n";
        _hi++;
    }
    Raw+="\r\n";
    Raw+=Body;
    std::string _r=Raw;
    return _r;
}

bool HttpResponse::status(int _code,const std::string & _str){
    Code=_code;
    CodeInfo=_str;
    return true;
}

bool HttpResponse::status(int _code){
    Code=_code;
    CodeInfo=HttpCodeStatus(_code);
    return true;
}

HttpResponse::~HttpResponse(){

}
