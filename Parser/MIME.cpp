#include "include/abxhttpd.H"
namespace abxhttpd{

    MIMEs MIMEList
    {
        {".pdf","application/pdf"},
        {".html","text/html"},
        {".htm","text/html"},
        {".js","text/js"},
        {".css","text/css"},
        {".jpg","image/jpeg"},
        {".gif","image/gif"},
        {".png","image/png"},
        {".ico","image/ico"},
        {".mp4","video/mp4"}
    };

    bool IsMIMEExist(std::string & _src){
        MIMEs::iterator _f;
        _f=MIMEList.find(_src);
        return (!(_f==MIMEList.end()));
    }

    std::string _GMIME(std::string & _suffix){
        if(IsMIMEExist(_suffix)){
            return MIMEList[_suffix];
        }
        return std::string("application/octet-stream");
    }

}