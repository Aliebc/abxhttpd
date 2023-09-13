#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include "include/HttpdTools.hxx"
#include "include/HttpResponse.hxx"
#include "include/HttpRequest.hxx"
#include "include/HttpdSession.hxx"
#include "include/Httpd.hxx"
#include "include/mimes.hxx"

abxhttpd::ModuleConfigure AX_HTTP_INFO={"http",{
    {"Support","enabled"},
    {"Module Version","AB.X HTTP Parser 0.2"},
    {"Protocol Version","HTTP/1.1"}
}};
abxhttpd::Module AX_HTTP_MODULE(&AX_HTTP_INFO);

namespace abxhttpd{

bool PathSecurityCheck(const std::string & _path){
    int _root = 0;
    for (int i = 0; i < _path.size(); i++) {
        if (_path.at(i) == '/') {
            _root++;
            if(i + 1 < _path.size() && _path.at(i + 1)=='.' && 
            i + 2 < _path.size() && _path.at(i + 2)=='.' && 
            i + 3 < _path.size() && _path.at(i + 3)=='/'){
                _root -= 2;
                i += 2;
            }else if(i + 1 < _path.size() && _path.at(i + 1)=='/'){
                _root--;
            }
        }
        if (_root < 0) {
            return false;
        }
    }
    return true;
}

ABXHTTPD_API void _DefaultHttpH(HttpResponse & Response,const HttpRequest & Request, void * _args){
    SocketRequest * _ssrc =static_cast<SocketRequest *>(_args);
    std::string _path(_ssrc->Http_S.Path+HttpdTools::ABX_URLDecode(Request.path()));
    if(Request.path().at(Request.path().size()-1)=='/'){
        _path.insert(_path.size(),"index.html");
    }
    std::string _suffix(HttpdTools::_FileSuffix(_path));
    if(Request.path()=="/abxhttpd"){
        Response.body(HttpdTools::ABXInfoPageHTML(&Request));
    }else{
        try{
            if(!PathSecurityCheck(_path)){
                throw HttpException(400);
            }
            Response.header("Content-Type",_GMIME(_suffix));
            Response.header("Content-Length",std::to_string(HttpdTools::_FileLength(_path)));
            Response.need_send_from_stream=true;
            Response.need_send_from_stream_src=(_path);
        }catch (const HttpException & _e){
            Response.status(_e.code());
            Response.need_send_from_stream=false;
            Response.header("Content-Type",_GMIME(".html"));
            Response.body(_e.html());
        }
    }
}

HttpHandler DefaultHttpH=_DefaultHttpH;
}
