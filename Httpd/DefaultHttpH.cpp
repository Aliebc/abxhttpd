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
abxhttpd::Module AX_HTTP_MODULE(&AX_HTTP_INFO);

namespace abxhttpd{
    HttpResponse _DefaultHttpH(HttpRequest & _src, void * _args){
        SocketRequest _ssrc =*(SocketRequest *)_args;
        HttpResponse _hresponse;
        _hresponse.header("Connection")=_src.is_header("Connection")?_src.header("Connection"):" close";
        if(_src.path().at(_src.path().size()-1)=='/'){
            _src.path().insert(_src.path().size(),"index.html");
        }
        std::string _path(_ssrc.Http_S.Path+HttpdTools::ABX_URLDecode(_src.path()));
        std::string _suffix(HttpdTools::_FileSuffix(_src.path()));
        _hresponse.status(200);
        if(_src.path()=="/abxhttpd"){
            _hresponse.body(ABXInfoPageHTML(_src));
        }else{
            try{
                _hresponse.header("Content-Type",_GMIME(_suffix));
                _hresponse.header("Content-Length",std::to_string(HttpdTools::_FileLength(_path)));
                _hresponse.need_send_from_stream=true;
                _hresponse.need_send_from_stream_src=(_path);
            }catch (const abxhttpd_error_http & _e){
                _hresponse.status(_e.code());
                _hresponse.need_send_from_stream=false;
                _hresponse.header("Content-Type",_GMIME(".html"));
                _hresponse.body(_e.html());
            }
        }
        return _hresponse;
    }

    HttpHandler DefaultHttpH=_DefaultHttpH;
}
