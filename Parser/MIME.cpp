#include "include/abxhttpd.H"
namespace abxhttpd{

    MIMEs MIMEList
    {
        {".pdf","application/pdf"},
        {".wasm","application/wasm"},
        {".html","text/html"},
        {".htm","text/html"},
        {".js","application/javascript"},
        {".css","text/css"},
        {".jpg","image/jpeg"},
        {".svg","image/svg+xml"},
        {".gif","image/gif"},
        {".png","image/png"},
        {".ico","image/x-icon"},
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
        return "application/octet-stream";
    }

    std::string _GMIME(const char * _suffix){
        std::string __suffix(_suffix);
        if(IsMIMEExist(__suffix)){
            return MIMEList[__suffix];
        }
        return "application/octet-stream";
    }

}
