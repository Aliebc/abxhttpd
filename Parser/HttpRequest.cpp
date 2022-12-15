#include "include/HttpRequest.hxx"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>
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

int HttpRequest::parse_header(){
    status_id=S_ID::NOT_FINISHED;
    string _src=Buffer;
    Length=_src.size();
    if(Length<8){
        throw HttpParserException(S_ID::NOT_FINISHED);
    }
    size_t _p1=0;
    size_t _p2=0;
    _p1=_src.find_first_of(' ');
    if(_p1==std::string::npos){
        throw HttpParserException(S_ID::UNKNOWN_PROTOCOL);
    }
    Method=_src.substr(0L,_p1);
    _src.erase(0L,_p1+1);
    _p1=_src.find_first_of(' ');
    _p2=_src.find_first_of('?');
    if(_p1>=Length){
        throw HttpParserException(S_ID::NOT_FINISHED);
    }
    if(_p1<=_p2){
        Path=_src.substr(0L,_p1);
    }else{
        Path=_src.substr(0L,_p2);
        QueryString=_src.substr(_p2+1,_p1-(_p2+1));
    }
    _src.erase(0L,_p1+1);
    _p1=_src.find_first_of('\n');
    if(_p1>=Length){
        throw HttpParserException(S_ID::NOT_FINISHED);
    }
    Protocol=_src.substr(0L,_p1);
    if(strncmp(Protocol.c_str(),"HTTP/",5)!=0){
        throw HttpParserException(S_ID::UNKNOWN_PROTOCOL);
    }
    if(_p1>=Length){
        throw HttpParserException(S_ID::NOT_FINISHED);
    }
    _src.erase(0L,_p1+1);
    while(true){
        _p1=_src.find_first_of('\n');
        if(_p1>=Length){
            throw HttpParserException(S_ID::NOT_FINISHED);
        }
        _p2=_src.find_first_of(':');
        if(_p2>=_p1){
            throw HttpParserException(S_ID::INVALID_FORMAT);
        }
        string _h(_src.substr(0L,_p2));
        string _rh=_src.substr(_p2+1,_p1-(_p2+1));
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
    status_id=0;
    Length=is_header("Content-Length")?atol(header("Content-Length").c_str()):0;
    return 0;
}

int HttpRequest::parse_body(){
    HttpdTools::ParseQueryString(GET, QueryString);
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

HttpRequest::HttpRequest(const string & _src){
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

const string & HttpRequest::method(void) const{
    return Method;
};

const string & HttpRequest::path(void) const{
    return Path;
};

string & HttpRequest::query_string(void){
    return QueryString;
};

const string & HttpRequest::body(void) const{
    return Body;
};

HttpRequest::HttpRequest(HttpRequest const & SS) {
    Buffer = SS.Buffer;
    SESSION = SS.SESSION;
    parse_header();
    parse_body();
}

const string & HttpRequest::request(const string && _key) const{
    auto _f=REQUEST.find(_key);
    if(_f!=REQUEST.end()){
        return _f->second;
    }
    return null_str;
}

SSMap & HttpRequest::cookies(){
    return COOKIE;
}

const string & HttpRequest::cookie(const string && _key) const{
    auto _f=COOKIE.find(_key);
    if(_f!=COOKIE.end()){
        return _f->second;
    }
    return null_str;
}

void HttpRequest::variables(const string & _key,const string & _val){
    ServerVariables[_key]=_val;
}

void HttpRequest::variables(const string & _key,const string && _val){
    ServerVariables[_key]=_val;
}

const string & HttpRequest::variables(const string && _key) const{
    auto _f=ServerVariables.find(_key);
    if(_f!=ServerVariables.end()){
        return _f->second;
    }
    return null_str;
}

const SSMap & HttpRequest::variables() const{
    return ServerVariables;
}

const string & HttpRequest::raw(void) const{
    return Buffer;
};

void HttpRequest::clear(){
    Buffer.clear();
    
}

SessionPtr * HttpRequest::session(const string && _key) const{
    if(SESSION==NULL){
        return NULL;
    }
    return &(*SESSION)[_key];
}

SessionPtr * HttpRequest::global(const string && _key) const{
    return &HttpdSession::global[_key];
}

size_t HttpRequest::size() const{
    return Body.size();
}

HttpRequest::~HttpRequest()=default;

}
