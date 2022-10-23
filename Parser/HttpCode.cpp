#include "include/HttpCode.hxx"

HttpCodeL HttpCodeList = {
    {200,"OK"},
    {403,"Forbidden"},
    {404,"Not Found"},
    {500,"Internal Error"},
};

namespace abxhttpd{

const std::string & HttpCodeStatus(int _code){
    return HttpCodeList[_code];
}

std::string HttpCodeStatusHTML(int _code){
    char _p[1024]={0};
    const char * _str=HttpCodeStatus(_code).c_str();
    snprintf(_p,sizeof(_p)-1,"<!DOCTYPE HTML>\n<html>\n\t<head>\n\t\t<title>\
    %d %s</title>\n\t</head>\n<body>\n\t<center><h1>%d %s</h1><hr> " ABXHTTPD_VERSION_SERVER "</center>\
    \n</body>\n</html>",_code,_str,_code,_str);
    return _p;
}

}
