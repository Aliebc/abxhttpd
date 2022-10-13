#include "include/HttpCode.hxx"

HttpCodeL HttpCodeList = {
    {200,"OK"},
    {403,"Forbidden"},
    {404,"Not Found"},
    {500,"Internal Error"},
};

const std::string & HttpCodeStatus(int _code){
    return HttpCodeList[_code];
}