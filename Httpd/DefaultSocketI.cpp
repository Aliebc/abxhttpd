#include "include/abxhttpd.H"
#include <ctime>
namespace abxhttpd{

#ifdef ABXHTTPD_UNIX

int _DefaultSocketI(SocketSettingList & _setting){
    
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked socket, returning %d",_sk);
    int optval=1;
    setsockopt(_sk,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked setsockopt.");
    struct sockaddr_in _in{0};
    struct in_addr bind_ip;
    if(inet_pton(AF_INET, _setting.IPStr,&bind_ip)==0){
        throw abxhttpd_error("Cannot Bind IP(Format).");
    }
    _in.sin_family=AF_INET;
    _in.sin_addr.s_addr=bind_ip.s_addr;
    _in.sin_port=htons(_setting.Port);
    int x1=bind(_sk,(struct sockaddr *)&_in,sizeof(_in));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked bind, returning %d",x1);
    int x2=listen(_sk,_setting.MaxConnect);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked listen, returning %d",x2);
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    _setting.allocated_socket=_sk;
    ABXHTTPD_INFO_PRINT(1,"[Core]Start listening on %s:%d",inet_ntoa(_in.sin_addr),_setting.Port);
    return _sk;
}

int _DefaultSocketI6(SocketSettingList & _setting){
    int _sk=socket(AF_INET6,SOCK_STREAM,0);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked socket, returning %d",_sk);
    int optval=1;
    int port = _setting.Port;
    setsockopt(_sk,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked setsockopt.");
    struct sockaddr_in6 _in6{0};
    struct in6_addr bind_ip;
    if(inet_pton(AF_INET6, _setting.IPStr,&bind_ip)==0){
        throw abxhttpd_error("Cannot Bind IP(Format).");
    }
    _in6.sin6_family=AF_INET6;
    _in6.sin6_addr=bind_ip;
    _in6.sin6_port=htons(_setting.Port);
    int x1=bind(_sk,(struct sockaddr *)&_in6,sizeof(_in6));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked bind, returning %d",x1);
    int x2=listen(_sk,_setting.MaxConnect);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked listen, returning %d",x2);
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    _setting.allocated_socket=_sk;
    ABXHTTPD_INFO_PRINT(1,"[Core]Start listening on %s:%d",_setting.IPStr,_setting.Port);
    return _sk;
}

#endif

#ifdef ABXHTTPD_WINDOWS

int _DefaultSocketI(SocketSettingList & _setting){
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked socket, returning %d",_sk);
    int port = _setting.Port;
    struct sockaddr_in _in{0};
    struct in_addr bind_ip;
    if(inet_pton(AF_INET, _setting.IPStr,&bind_ip)==0){
        throw abxhttpd_error("Cannot Bind IP(Format).");
    }
    _in.sin_family=AF_INET;
    _in.sin_addr.s_addr=bind_ip.s_addr;
    _in.sin_port=htons(_setting.Port);
    int x1=bind(_sk,(struct sockaddr *)&_in,sizeof(_in));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked bind, returning %d",x1);
    int x2=listen(_sk,_setting.MaxConnect);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked listen, returning %d",x2);
    if(x2==SOCKET_ERROR){
        if(WSAGetLastError()==WSAEADDRINUSE){
            throw abxhttpd_error("Address already in use.");
        }else{
            throw abxhttpd_error(std::string("Windows unknown error,code: ")+std::to_string(WSAGetLastError()));
        }
    }
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    _setting.allocated_socket=_sk;
    ABXHTTPD_INFO_PRINT(1,"[Core]Start listening on %s:%d",inet_ntoa(_in.sin_addr),port);
    return _sk;
}

int _DefaultSocketI6(SocketSettingList & _setting){
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked socket, returning %d",_sk);
    int port = _setting.Port;
    struct sockaddr_in6 _in6{0};
    struct in6_addr bind_ip;
    if(inet_pton(AF_INET6, _setting.IPStr,&bind_ip)==0){
        throw abxhttpd_error("Cannot Bind IP(Format).");
    }
    _in6.sin6_family=AF_INET6;
    _in6.sin6_addr=bind_ip;
    _in6.sin6_port=htons(_setting.Port);
    int x1=bind(_sk,(struct sockaddr *)&_in6,sizeof(_in6));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked bind, returning %d",x1);
    int x2=listen(_sk,_setting.MaxConnect);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked listen, returning %d",x2);
    if(x2==SOCKET_ERROR){
        if(WSAGetLastError()==WSAEADDRINUSE){
            throw abxhttpd_error("Address already in use.");
        }else{
            throw abxhttpd_error(std::string("Windows unknown error,code: ")+std::to_string(WSAGetLastError()));
        }
    }
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    _setting.allocated_socket=_sk;
    ABXHTTPD_INFO_PRINT(1,"[Core]Start listening on %s:%d",_setting.IPStr,_setting.Port);
    return _sk;
}

#endif

int _DefaultSocketD(SocketSettingList & _setting){
    return HttpdSocket::__close_socket(_setting.allocated_socket);
}

ConfigureInfo Socket6A{"ipv6",{{"Support","enabled"}}};
Module Socket6(&Socket6A);

int __DefaultSocketI(SocketSettingList & _setting){
    int _sid;
    if(_setting.IPVer==4){
        _sid=_DefaultSocketI(_setting);
    }else if (_setting.IPVer==6){
        _sid=_DefaultSocketI6(_setting);
    }else{
        throw abxhttpd_error("Unexpected Error.");
    }
    return _sid;
}

SocketInitializer DefaultSocketI=__DefaultSocketI;
SocketDestructor DefaultSocketD=_DefaultSocketD;

}
