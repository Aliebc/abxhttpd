#include "include/HttpResponse.hxx"
#include "include/HttpCode.hxx"
#include <stdio.h>
#include <iostream>
using namespace abxhttpd;

HttpResponse::HttpResponse(){
    header("Server",ABXHTTPD_VERSION_SERVER);
    header("Connection", "close");
    header("Content-Type","text/html; charset=utf-8");
    Protocol=ABXHTTPD_DEFAULT_PROTOCOL;
    Code=200;
}

HttpResponse::HttpResponse(const std::string & _src):HttpResponse(){
    Body=_src;
    Code=200;
}

HttpResponse::HttpResponse(const std::string & _src,int code):HttpResponse(_src){
    status(code);
}

HttpResponse::HttpResponse(const std::string & _src,int code,HttpHeaders headers):
HttpResponse(_src,code){
    Headers=headers;
}

HttpResponse::HttpResponse(const char * _src,size_t _len):HttpResponse(){
    Body=std::string(_src,_len);
    Code=200;
}

HttpResponse::HttpResponse(const char * _src):HttpResponse(){
    Body=std::string(_src);
    Code=200;
}

std::string & HttpResponse::body(void){
    return Body;
}

void HttpResponse::body(const std::string &&_src){
    Body=std::move(_src);
}

std::string HttpResponse::raw(void){
    std::stringstream _rr;
    if(header("Content-Length").size()==0){
        header("Content-Length",std::to_string(Body.size()));
    }
    _rr << Protocol << " " << std::to_string(Code) << " " << CodeInfo << "\r\n";
    {
        auto _hi=headers().begin();
        while(_hi!=headers().end()){
            _rr << _hi->first << ": " << _hi->second << "\r\n";
            ++_hi;
        }
    }
    _rr << "\r\n" << Body;
    return _rr.str();
}

void HttpResponse::set_cookie(const std::string & key,
                const std::string & value,
                long expires,
                const std::string & path
                ){
    std::stringstream tmp;
    tmp << key << "=" << value << "; ";
    tmp << "Path=" << path << "; ";
    header("Set-Cookie", tmp.str());
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

void HttpResponse::write(const std::string && _src){
    Body+=_src;
}

void HttpResponse::location(std::string && _loc){
    throw HttpException(302,_loc.c_str());
}

HttpResponse::~HttpResponse(){

}
