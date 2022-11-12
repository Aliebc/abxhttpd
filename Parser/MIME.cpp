#include "include/mimes.hxx"

#define ABXHTTPD_MIME_DEFAULT "application/octet-stream"
namespace abxhttpd{
    MIMEs MIMEList
    {
        {".pdf","application/pdf"},
        {".wasm","application/wasm"},
        {".html","text/html"},
        {".xml","text/xml"},
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

    bool IsMIMEExist(const std::string & _src){
        return (!(MIMEList.find(_src)==MIMEList.end()));
    }

    std::string _GMIME(const std::string & _suffix){
        return IsMIMEExist(_suffix)?MIMEList[_suffix]:ABXHTTPD_MIME_DEFAULT;
    }

    std::string _GMIME(const char * _suffix){
        std::string __suffix(_suffix);
        return IsMIMEExist(__suffix)?MIMEList[__suffix]:ABXHTTPD_MIME_DEFAULT;
    }

}
