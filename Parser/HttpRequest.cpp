#include "include/abxhttpd.H"
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

void HttpRequest::parse(void){
    std::string _src(Raw);
    state=false;
    Length=_src.size();
    HttpRequest_Parser_Assert(Length > 5,0LU,"Http request too short.");
    HttpRequest_Parser_Assert(Length < ULONG_MAX,0LU,"Http request too long.");
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
        Query_string=_src.substr(_p2+1,_p1-(_p2+1));
    }
    _src.erase(0L,_p1+1);
    _p1=_src.find_first_of('\n');
    HttpRequest_Parser_Assert(_p1 < Length,_p1,"Reach end of request.");
    Protocol=_src.substr(0L,_p1);
    HttpRequest_Parser_Assert(!strncmp(Protocol.c_str(),"HTTP/",5),_p1,"Unknown protocol.");
    _src.erase(0L,_p1+1);
    while(true){
        _p1=_src.find_first_of('\n');
        HttpRequest_Parser_Assert(_p1 < Length,_p1,"Reach end of request.");
        _p2=_src.find_first_of(':');
        HttpRequest_Parser_Assert(_p2<_p1,_p1,"Header format error.");
        std::string _h=_src.substr(0L,_p2);
        std::transform(_h.begin(),_h.end(),_h.begin(),(int(*)(int))toupper_s);
        std::string _rh=_src.substr(_p2+1,_p1-(_p2+1));
        size_t _p3=_rh.find_first_of('\r');
        size_t _p4=_rh.find_first_of(' ');
        Headers[_h]=_rh.substr(_p4+1,_p3-(_p4+1));
        _src.erase(0L,_p1+1);
        if(_src[0]=='\n' || (_src[0]=='\r' && _src[1]=='\n')){
            _src.erase(0L,2L);
            break;
        }
    }
    Body=_src;
    state=true;
}

HttpRequest::HttpRequest(const std::string & _src){
    state=false;
    Raw=_src;
}

HttpRequest::HttpRequest(){
    state=false;
}

HttpRequest::HttpRequest(const char * _src,size_t _len){
    state=false;
    Raw.append(_src,_len);
}

HttpRequest::HttpRequest(const char * _src){
    state=false;
    Raw.append(_src);
}

std::string & HttpRequest::method(void){
    return Method;
};

const std::string & HttpRequest::remote_addr(void){
    return Remote_addr;
};

void HttpRequest::remote_addr(const std::string & _ip){
    Remote_addr=std::move(_ip);
}

std::string & HttpRequest::path(void){
    return Path;
};

std::string & HttpRequest::query_string(void){
    return Query_string;
};

std::string & HttpRequest::body(void){
    return Body;
};

std::string & HttpRequest::header(const char * _header){
    std::string _h(_header);
    std::transform(_h.begin(),_h.end(),_h.begin(),(int(*)(int))toupper_s);
    return Headers[_h];
}

std::string & HttpRequest::header(const std::string & _header){
    std::string _h(_header);
    std::transform(_h.begin(),_h.end(),_h.begin(),(int(*)(int))toupper_s);
    return Headers[_h];
}

HttpRequest::HttpRequest(HttpRequest const & SS) {
    Raw = SS.Raw;
    parse();
}

StrArray & HttpRequest::headers(void){
    return Headers;
}

std::string & HttpRequest::raw(void){
    return Raw;
};

void HttpRequest::clear(){
    Raw.clear();
    
}

bool HttpRequest::is_header(const char * _header){
    return (header(_header).size()!=0);
}

HttpRequest::~HttpRequest(){};

}
