#include "include/HttpRequest.hxx"
#include <cstring>
#include <algorithm>
#ifndef ULONG_MAX
#define ULONG_MAX (1ULL<<(8*sizeof(size_t)-1))
#endif

namespace abxhttpd{

int toupper_s(int _s){
    if((char)_s=='-'){
        return '_';
    }
    return toupper(_s);
}

int HttpRequest::parse(void){
    std::string _src(Buffer);
    Length=_src.size();
    HttpRequest_Parser_Assert(Length > 5,0LU,"Http request too short.");
    //throw BasicHttpException(BasicHttpException::S_ID::NOT_FINISHED);
    size_t _p1=0;
    size_t _p2=0;
    _p1=_src.find_first_of(' ');
    HttpRequest_Parser_Assert(_p1 < Length,_p1,"Reach end of request.");
    Method=_src.substr(0L,_p1);
    _src.erase(0L,_p1+1);
    _p1=_src.find_first_of(' ');
    _p2=_src.find_first_of('?');
    HttpRequest_Parser_Assert(_p1 < Length,_p1,"Reach end of request.");
    if(_p1<=_p2){
        Path=_src.substr(0L,_p1);
    }else{
        Path=_src.substr(0L,_p2);
        Query_String=_src.substr(_p2+1,_p1-(_p2+1));
    }
    _src.erase(0L,_p1+1);
    _p1=_src.find_first_of('\n');
    HttpRequest_Parser_Assert(_p1 < Length,_p1,"Reach end of request.");
    Protocol=_src.substr(0L,_p1);
    HttpRequest_Parser_Assert(!strncmp(Protocol.c_str(),"HTTP/",5),_p1,"Unknown protocol.");
    _src.erase(0L,_p1+1);
    status_id=S_ID::NOT_FINISHED;
    while(true){
        _p1=_src.find_first_of('\n');
        HttpRequest_Parser_Assert(_p1 < Length,_p1,"Reach end of request.");
        _p2=_src.find_first_of(':');
        HttpRequest_Parser_Assert(_p2<_p1,_p1,"Header format error.");
        std::string _h(_src.substr(0L,_p2));
        std::string _rh=_src.substr(_p2+1,_p1-(_p2+1));
        size_t _p3=_rh.find_first_of('\r');
        size_t _p4=_rh.find_first_of(' ');
        header(_h,_rh.substr(_p4+1,_p3-(_p4+1)));
        std::transform(_h.begin(), _h.end(), _h.begin(), toupper_s);
        _h.insert(0, "HTTP_");
        ServerVariables[_h]=_rh.substr(_p4+1,_p3-(_p4+1));
        _src.erase(0L,_p1+1);
        if(_src[0]=='\n' || (_src[0]=='\r' && _src[1]=='\n')){
            _src.erase(0L,2L);
            break;
        }
    }
    Body=_src;
    HttpdTools::ParseQueryString(GET, Query_String);
    REQUEST=GET;
    if(Method=="POST"){
        if(header("Content-Type")=="application/x-www-form-urlencoded"){
            HttpdTools::ParseQueryString(POST, Body);
            for(auto _i=POST.begin();_i!=POST.end();++_i){
                REQUEST[_i->first]=_i->second;
            }
        }
    }
    HttpdTools::ParseCookie(COOKIE, header("Cookie"));
    if(cookie(ABXHTTPD_SESSION_STR).size()==0){
        SESSION =  &HttpdSession::get(HttpdSession::allocate());
    }else{
        SESSION = &HttpdSession::get(cookie(ABXHTTPD_SESSION_STR));
    }
    return 0;
}

HttpRequest::HttpRequest(const std::string & _src){
    Buffer=_src;
}

HttpRequest::HttpRequest(){
}

HttpRequest::HttpRequest(const char * _src,size_t _len){
    Buffer.append(_src,_len);
}

HttpRequest::HttpRequest(const char * _src){
    Buffer.append(_src);
}

const std::string & HttpRequest::method(void) const{
    return Method;
};

const std::string & HttpRequest::path(void) const{
    return Path;
};

std::string & HttpRequest::query_string(void){
    return Query_String;
};

const std::string & HttpRequest::body(void) const{
    return Body;
};

HttpRequest::HttpRequest(HttpRequest const & SS) {
    Buffer = SS.Buffer;
    parse();
}

const std::string & HttpRequest::request(const std::string && _key) const{
    auto _f=REQUEST.find(_key);
    if(_f!=REQUEST.end()){
        return _f->second;
    }
    return null_str;
}

SSMap & HttpRequest::cookies(){
    return COOKIE;
}

const std::string & HttpRequest::cookie(const std::string && _key) const{
    auto _f=COOKIE.find(_key);
    if(_f!=COOKIE.end()){
        return _f->second;
    }
    return null_str;
}

void HttpRequest::variables(const std::string & _key,const std::string & _val){
    ServerVariables[_key]=_val;
}

void HttpRequest::variables(const std::string & _key,const std::string && _val){
    ServerVariables[_key]=_val;
}

const std::string & HttpRequest::variables(const std::string && _key) const{
    auto _f=ServerVariables.find(_key);
    if(_f!=ServerVariables.end()){
        return _f->second;
    }
    return null_str;
}

const SSMap & HttpRequest::variables() const{
    return ServerVariables;
}

const std::string & HttpRequest::raw(void) const{
    return Buffer;
};

void HttpRequest::clear(){
    Buffer.clear();
    
}

SessionPtr * HttpRequest::session(const std::string && _key) const{
    if(SESSION==NULL){
        return NULL;
    }
    return &(*SESSION)[_key];
}

SessionPtr * HttpRequest::global(const std::string && _key) const{
    return &HttpdSession::global[_key];
}

HttpRequest::~HttpRequest(){
};

}
