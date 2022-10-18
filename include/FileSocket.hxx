#ifndef FILE_SOCKET_H
#define FILE_SOCKET_H

#include <stdio.h>
#include "HttpdSocket.hxx"

namespace abxhttpd {
class FileSocket:public HttpdSocket{
private:
    FILE * fp;
    char tmp[1024];
public:
    FileSocket(const char * path){
        fp=fopen(path,"rb");
        if(fp==NULL){
            
        }
    }
};
}

#endif
