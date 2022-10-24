#ifndef HTTPD_TOOLS_H
#define HTTPD_TOOLS_H

#define ABXHTTPD_MIN(a,b) (a<b?a:b)

namespace abxhttpd {
    
    size_t _FileLength(std::string & _Path);
    std::string _FileSuffix(std::string & _Name);
    std::string ABX_URLDecode(std::string & _src);
    template <class SocketStream>
    void * _ThreadHandler(void * _ptr);
    template <class SocketStream>
    void * _ThreadController (const ThreadSettingList & _set, const CCore & _core, void * _args);
    std::string ABXInfoPageHTML(HttpRequest & _req);
}
#endif
