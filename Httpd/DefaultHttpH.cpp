#include "include/abxhttpd.H"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include "include/HttpdTools.hxx"

abxhttpd::ConfigureInfo AX_HTTP_INFO={"http",{
    {"Support","enabled"},
    {"Module Version","AB.X HTTP Parser 0.1"},
    {"Protocol Version","HTTP/1.1"}
}};
abxhttpd::Module AX_HTTP_MODULE(AX_HTTP_INFO);

typedef struct {
    char _src[1<<12];
    long int _p;
} ENV_TMP;

#define putenv_s(_env_tmp,_env_str,...) {\
    int _slen=sprintf((_env_tmp)->_src+(_env_tmp)->_p,_env_str,##__VA_ARGS__);\
    putenv((_env_tmp)->_src+(_env_tmp)->_p);\
    (_env_tmp)->_p+=_slen+1;\
}

namespace abxhttpd{
    HttpResponse _PHPCGI(std::string & _CGIP, HttpRequest & _Hreq, void * _args);
}

namespace abxhttpd{
    HttpResponse _DefaultHttpH(HttpRequest & _src, void * _args){
        SocketRequest _ssrc =*(SocketRequest *)_args;
        HttpResponse _hr;
        if(_src.path().at(_src.path().size()-1)=='/'){
            _src.path().insert(_src.path().size(),"index.html");
        }
        std::string _path=std::string(_ssrc.Http_S.Path+ABX_URLDecode(_src.path()));
        std::string _suffix;
        _suffix=_FileSuffix(_src.path());
        if(_src.path()=="/abxhttpd"){
            _hr.status(200);
            _hr.body()=ABXInfoPageHTML(_src);
            _hr.header("Content-Length")=std::string(" ")+std::to_string(_hr.body().size());
        }else{
            try{
                _hr.header("Content-Type")=_GMIME(_suffix);
                size_t x=_FileLength(_path);
                _hr.header("Content-Length")=std::to_string(x);
                _hr.need_send_from_stream=true;
                _hr.need_send_from_stream_src=(_path);
            }catch (abxhttpd_error_http & _e){
                _hr.status(_e.code());
                std::string _suffix(".html");
                _hr.need_send_from_stream=false;
                _hr.header("Content-Type")=_GMIME(_suffix);
                _hr.body()=_e.html();
                _hr.header("Content-Length")=std::string(" ")+std::to_string(_hr.body().size());
            }
        }
        _hr.header("Connection")=_src.is_header("Connection")?_src.header("Connection"):std::string("close");
        _hr.header("Server")=std::string(" " ABXHTTPD_VERSION_SERVER);
        return _hr;
    }

    std::string _PipeRead(std::string _Path,std::string _Write){
        std::string res;
        /*FILE * _pipe=popen(_Path.c_str(),"r+");
        if(_pipe==NULL){
            throw abxhttpd_error("Cannot open pipe");
        }
        fwrite(_Write.c_str(),sizeof(char),_Write.size(),_pipe);
        char _pc[24];
        size_t _rd=0;
        while(true){
            _rd=fread(_pc,sizeof(char),sizeof(_pc),_pipe);
            if(_rd>0){
                res+=std::string(_pc,_rd);
            }else{
                break;
            }
            if(_rd==sizeof(_pc)){
                continue;
            }else{
                break;
            }
        }*/
        return res;
    }

    /*HttpResponse _PHPCGI(std::string & _CGIP, HttpRequest & _Hreq, void * _args){
        HttpResponse RET;
        ENV_TMP env_tmp={0};
        SocketRequest _ssrc =*(SocketRequest *)_args;
        std::string _Path=_ssrc.Http_S.Path+_Hreq.path();
        putenv_s(&env_tmp,"GATEWAY_INTERFACE=CGI/1.1");
        putenv_s(&env_tmp,"SERVER_PROTOCOL=HTTP/1.1");
        putenv_s(&env_tmp,"CONTENT_LENGTH=%ld",_Hreq.body().size());
        putenv_s(&env_tmp,"SERVER_SOFTWARE=" ABXHTTPD_VERSION_SERVER);
        putenv_s(&env_tmp,"REQUEST_METHOD=%s",_Hreq.method().c_str());
        putenv_s(&env_tmp,"SCRIPT_FILENAME=%s",_Path.c_str());
        putenv_s(&env_tmp,"SCRIPT_NAME=%s",_Hreq.path().c_str());
        putenv_s(&env_tmp,"REMOTE_ADDR=%s",_Hreq.remote_addr().c_str());
        putenv_s(&env_tmp,"QUERY_STRING=%s",_Hreq.query_string().c_str());
        putenv_s(&env_tmp,"CONTENT_TYPE=%s",_Hreq.header("Content-Type").c_str());
        StrArray::iterator _h=_Hreq.headers().begin();
        while(true){
            putenv_s(&env_tmp,"HTTP_%s=%s",_h->first.c_str(),_h->second.c_str());
            _h++;
            if(_h==_Hreq.headers().end()){
                break;
            }
        }
        std::string cgi_str=_PipeRead(_CGIP,_Hreq.body());
        size_t _p1;
        while(true){
            _p1=cgi_str.find_first_of('\n');
            cgi_str.erase(0L,_p1+1);
            if(cgi_str[0]=='\n'||cgi_str[0]=='\r'){
                cgi_str.erase(0L,2L);
                break;
            }else if(cgi_str.size()==0){
                break;
            }
        }
        RET.header("Content-Type")=" text/html; charset=utf-8";
        RET.body()=cgi_str;
        return RET;
    }*/

    HttpHandler DefaultHttpH=_DefaultHttpH;
}
