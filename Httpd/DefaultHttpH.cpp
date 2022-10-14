#include "include/abxhttpd.H"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdarg.h>

ABXHTTPD_MODINITFUNC AX_HTTP(){
    abxhttpd::RegisterModule( "HttpParserEngine" , "AB.X HTTP Parser");
}

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
    std::string _DefaultCodePage(int _code);
    std::string _FileRead(std::string _Path);
    std::string _FileSuffix(std::string _Name);
    std::string ABX_URLDecode(std::string & _src);
    HttpResponse _PHPCGI(std::string & _CGIP, HttpRequest & _Hreq, void * _args);
}

namespace abxhttpd{
    HttpResponse _DefaultHttpH(HttpRequest & _src, void * _args){
        SocketRequest _ssrc =*(SocketRequest *)_args;
        HttpResponse _hr;
        std::string _suffix;
        _suffix=_FileSuffix(_src.path());
        if(_suffix==".php"){
            try{
                std::string _b=_FileRead(_ssrc.Http_S.Path+_src.path());
                std::string _PHPCGIPATH("/usr/local/php/bin/php-cgi");
                _hr=_PHPCGI(_PHPCGIPATH,_src,_args);
            }catch(abxhttpd_error _e){
                 _hr.status(500);
                _suffix=std::string(".html");
                _hr.header("Content-Type")=_GMIME(_suffix);
                _hr.body()=_DefaultCodePage(500);
            }
            goto END;
        }
        if(_src.path()=="/abxhttpd"){
            _hr.status(200);
            _hr.body()=std::string(ABXHTTPD_INFO_PAGE);
        }else{
            try{
                std::string _b=_FileRead(_ssrc.Http_S.Path+ABX_URLDecode(_src.path()));
                _hr.header("Content-Type")=_GMIME(_suffix);
                _hr.body()=_b;
            }catch (abxhttpd_error _e){
                _hr.status(404);
                _suffix=std::string(".html");
                _hr.header("Content-Type")=_GMIME(_suffix);
                _hr.body()=_DefaultCodePage(404);
            }
        }
END:       _hr.header("Connection")=std::string(" close");
        _hr.header("Content-Length")=std::string(" ")+std::to_string(_hr.body().size());
        _hr.header("Receive-Size")=std::to_string(_src.body().size());
        _hr.header("Server")=std::string(" " ABXHTTPD_VERSION_SERVER);
        return _hr;
    }

    std::string _DefaultCodePage(int _code){
        char _p[2048];
        const char * _str=HttpCodeStatus(_code).c_str();
        sprintf(_p,"<!DOCTYPE HTML>\n<html>\n\t<head>\n\t\t<title>\
        %d %s</title>\n\t</head>\n<body>\n\t<center><h1>%d %s</h1><hr> " ABXHTTPD_VERSION_SERVER "</center>\
        \n</body>\n</html>",_code,_str,_code,_str);
        return std::string(_p);
    }

    HttpResponse _AbxhttpdPage(void){
        HttpResponse _hr;
        _hr.body()=std::string(ABXHTTPD_INFO_PAGE);
        return _hr;
    }

    std::string _FileRead(std::string _Path){
        std::string fps;
        FILE * fp=fopen(_Path.c_str(),"rb");
        if(fp==NULL){
            throw abxhttpd_error("File not exists");
        }
        #ifdef ABXHTTPD_UNIX
        struct stat _ft;
        stat(_Path.c_str(),&_ft);
        if(!S_ISREG(_ft.st_mode)){
            throw abxhttpd_error("Not Regular File");
        }
        char _pc[4096];
        size_t _rd=0;
        size_t _ed=_ft.st_size;
        size_t _pd=0;
        #else
        DWORD st =GetFileAttributes(_Path.c_str());
        if(st&FILE_ATTRIBUTE_DIRECTORY){
            throw abxhttpd_error("Not Regular File");
        }
        size_t f_size;
        char _pc[4096];
        fseek(fp,0,SEEK_END);
        f_size=ftell(fp);
        size_t _rd=0;
        size_t _ed=f_size;
        size_t _pd=0;
        rewind(fp);
        #endif
        while(true){
            _rd=fread(_pc,sizeof(char),sizeof(_pc),fp);
            if(_pd==_ed){
                break;
            }
            fps+=std::string(_pc,_rd);
            memset(_pc,0,sizeof(_pc));
            _pd+=_rd;
        }
        fclose(fp);
        return fps;
    }

    /*std::string _FileRead_B(std::string _Path){
        std::string fps;
        FILE * fp=fopen(_Path.c_str(),"rb");
        if(fp==NULL){
            throw abxhttpd_error("File not exists");
        }
        struct stat _ft;
        stat(_Path.c_str(),&_ft);
        if(!S_ISREG(_ft.st_mode)){
            throw abxhttpd_error("Not Regular File");
        }
        char _pc[4096];
        size_t _rd=0;
        size_t _ed=_ft.st_size;
        size_t _pd=0;
        while(true){
            _rd=fread(_pc,sizeof(char),sizeof(_pc),fp);
            if(_pd==_ed){
                break;
            }
            fps+=std::string(_pc,_rd);
            memset(_pc,0,sizeof(_pc));
            _pd+=_rd;
        }
        fclose(fp);
        return fps;
    }*/

    std::string _FileSuffix(std::string _Name){
        size_t _Dot=_Name.find_last_of('.');
        size_t _Len=_Name.size();
        if(std::string::npos==_Dot){
            return std::string();
        }
        return _Name.substr(_Dot,_Len-_Dot);
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

    std::string _ABX_URLDecode(std::string & _src){
        std::string res;
        auto src=_src.begin();
        while(src!=_src.end()){
            if(*(src)!='%'){
                res.append(1,*src);
                src++;
                continue;
            }else{
                int a;
                sscanf(_src.c_str()+(src-_src.begin())+1,"%2X",&a);
                a-=256;
                res.append(1,static_cast<char>(a));
                src+=3;
            }
        }
        return res;
    }

    #ifdef ABXHTTPD_WINDOWS
    char* Utf8ToGB2312(const char* utf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
 
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
 
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
 
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
 
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
 
		if (wstr) {
			delete[] wstr;
			wstr = NULL;
		}
		return str;
	}
    std::string ABX_URLDecode(std::string & _src){
        std::string utf_8_c(_ABX_URLDecode(_src));
        char * gbk_c=Utf8ToGB2312(utf_8_c.c_str());
        std::string res(gbk_c);
        delete[] gbk_c;
        return res;
    }
    #else
    std::string ABX_URLDecode(std::string & _src){
        return _ABX_URLDecode(_src);
    }
    #endif

    HttpResponse _PHPCGI(std::string & _CGIP, HttpRequest & _Hreq, void * _args){
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
    }

    HttpHandler DefaultHttpH=_DefaultHttpH;
}
