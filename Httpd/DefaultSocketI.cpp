#include "include/abxhttpd.H"
#include <ctime>
namespace abxhttpd{

#ifdef ABXHTTPD_UNIX

int _DefaultSocketI(SocketSettingList & _setting){
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked socket, returning %d",_sk);
    int optval=1;
    int port = _setting.Port;
    if(_setting.Is_reused){
        setsockopt(_sk,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int));
        ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked setsockopt.");
    }
    struct sockaddr_in _in={0};
    _in.sin_family=AF_INET;
    _in.sin_addr.s_addr=(_setting.Bind_IP);
    _in.sin_port=htons(port);
    int x1=bind(_sk,(struct sockaddr *)&_in,sizeof(_in));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked bind, returning %d",x1);
    int x2=listen(_sk,_setting.Max_connect_count);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked listen, returning %d",x2);
    if(!(x1>=0&&x2>=0)){
        throw abxhttpd_error(std::string(strerror(errno)));
    }
    _setting.allocated_socket=_sk;
    ABXHTTPD_INFO_PRINT(1,"[Core]Start listening on %s:%d",inet_ntoa(_in.sin_addr),port);
    return _sk;
}

#endif

#ifdef ABXHTTPD_WINDOWS

int _DefaultSocketI(SocketSettingList & _setting){
    WSADATA wsadata;
    int wsa_c=WSAStartup(MAKEWORD(2,2),&wsadata);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked WSAStartup, returning %d",wsa_c);
    if(wsa_c!=0){
        throw abxhttpd_error("Cannot invoke WSAStartup");
    }
    int _sk=socket(AF_INET,SOCK_STREAM,0);
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked socket, returning %d",_sk);
    char optval=1;
    int port = _setting.Port;
    struct sockaddr_in _in={0};
    _in.sin_family=AF_INET;
    _in.sin_addr.s_addr=(_setting.Bind_IP);
    _in.sin_port=htons(port);
    unsigned long ul=1;
    //int _ws=ioctlsocket(_sk,FIONBIO,(unsigned long *)&ul);
    int x1=bind(_sk,(struct sockaddr *)&_in,sizeof(_in));
    ABXHTTPD_INFO_PRINT(11,"[Core][System API]Invoked bind, returning %d",x1);
    int x2=listen(_sk,_setting.Max_connect_count);
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

#endif

int _close_socket(int ad, int flag=0){
    int st;
    #ifdef ABXHTTPD_WINDOWS
    st=closesocket(ad);
    ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked closesocket, returning %d.",ad,st);
    #endif
    #ifdef ABXHTTPD_UNIX
    st=close(ad);
    ABXHTTPD_INFO_PRINT(11,"[Socket %d][System API]Invoked close, returning %d.",ad,st);
    if(st==0){
        ABXHTTPD_INFO_PRINT(3,"[Socket %d]Closed.",ad);
    }else{
        ABXHTTPD_INFO_PRINT(3,"Cannot close socket %d(Error code:%d)",ad,st);
    }
    #endif
    return st;
}

int _DefaultSocketD(SocketSettingList & _setting){
    return _close_socket(_setting.allocated_socket);
}

SocketInitializer DefaultSocketI=_DefaultSocketI;
SocketDestructor DefaultSocketD=_DefaultSocketD;

}
