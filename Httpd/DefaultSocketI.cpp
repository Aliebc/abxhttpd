#include "include/abxhttpd.H"
#include <ctime>
namespace abxhttpd{

#ifdef ABXHTTPD_UNIX

int _DefaultSocketI(const SocketSettingList & _setting){
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    int optval=1;
    int port = _setting.Port;
    if(_setting.Is_reused){
        setsockopt(_sk,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int));
    }
    struct sockaddr_in _in={0};
    _in.sin_family=AF_INET;
    _in.sin_addr.s_addr=(_setting.Bind_IP);
    
    _in.sin_port=htons(port);
    int x1=bind(_sk,(struct sockaddr *)&_in,sizeof(_in));
    int x2=listen(_sk,_setting.Max_connect_count);
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    return _sk;
}

#endif

#ifdef ABXHTTPD_WINDOWS

int _DefaultSocketI(const SocketSettingList & _setting){
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    char optval=1;
    int port = _setting.Port;
    if(_setting.Is_reused){
        setsockopt(_sk,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int));
    }
    struct sockaddr_in _in={0};
    _in.sin_family=AF_INET;
    _in.sin_addr.s_addr=(_setting.Bind_IP);
    _in.sin_port=htons(port);
    unsigned long ul=1;
    int _ws=ioctlsocket(_sk,FIONBIO,(unsigned long *)&ul);
    int x1=bind(_sk,(struct sockaddr *)&_in,sizeof(_in));
    int x2=listen(_sk,_setting.Max_connect_count);
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    return _sk;
}

#endif

SocketInitializer DefaultSocketI=_DefaultSocketI;

}