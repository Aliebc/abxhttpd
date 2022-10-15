#ifndef HTTPD_TOOLS_H
#define HTTPD_TOOLS_H
namespace abxhttpd {
    typedef struct _IPPortBin
    {
        int port;
    }IPPortBin;
    std::string _DefaultCodePage(int _code);
    std::string _FileRead(std::string _Path);
    std::string _FileSuffix(std::string _Name);
    std::string ABX_URLDecode(std::string & _src);
    template <class SocketStream>
    void * _ThreadHandler(void * _ptr);
    template <class SocketStream>
    void * _ThreadController (const ThreadSettingList & _set, const CCore & _core, void * _args);
}
#endif