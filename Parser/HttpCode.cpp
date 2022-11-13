#include "include/HttpCode.hxx"

namespace abxhttpd{

HttpCodeL HttpCodeList = {
    {200,"OK"},
    {301,"Permanently Moved"},
    {302,"Temporarily Moved"},
    {400,"Bad Request"},
    {401,"Unauthorized"},
    {403,"Forbidden"},
    {404,"Not Found"},
    {405,"Method Not Allowed"},
    {406,"Not Acceptable"},
    {500,"Internal Error"},
};

const std::string & HttpCodeStatus(int _code){
    if(HttpCodeList.find(_code)==HttpCodeList.end()){
        return HttpCodeList.at(400);
    }
    return HttpCodeList.at(_code);
}

std::string HttpCodeStatusHTML(int _code){
    char _p[1024]{0};
    const char * _str=HttpCodeStatus(_code).c_str();
    snprintf(_p,sizeof(_p)-1,"<!DOCTYPE HTML>\n<html>\n\t<head>\n\t\t<title>%d %s</title>\n\t</head>\n<body>\n\t<center><h1>%d %s</h1><hr>" ABXHTTPD_VERSION_SERVER "</center>\
    \n</body>\n</html>",_code,_str,_code,_str);
    return _p;
}

}
