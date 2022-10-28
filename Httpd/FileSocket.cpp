#include "include/HttpdSocket.hxx"
#include "include/FileSocket.hxx"

namespace abxhttpd{
FileSocket::FileSocket(const char * path):HttpdSocket(){
    fp=fopen(path,"rb");
    if(fp==NULL){
        throw abxhttpd_error("Not Regular File");
    }
    get_file_length();
    ABXHTTPD_ZERO_MEMORY(tmp);
}
FileSocket::~FileSocket(){
    fclose(fp);
}
void FileSocket::get_file_length(){
    fseek(fp, 0, SEEK_END);
    length=ftell(fp);
    rewind(fp);
}
size_t FileSocket::get_length(){
    return length;
}
size_t FileSocket::read(std::string &dst,size_t size){
    if(size==0){size--;}
    if(feof(fp)){
        return 0;
    }
    size_t _read_size=ABXHTTPD_MIN(size, length);
    size_t _read_len=0;
    size_t _read_all=0;
    while((_read_size/sizeof(tmp))>0){
        _read_len=fread(tmp, sizeof(char), sizeof(tmp), fp);
        dst.insert(dst.size(), tmp, _read_len);
        _read_size-=_read_len;
        _read_all+=_read_len;
        ABXHTTPD_ZERO_MEMORY(tmp);
    }
    _read_len=fread(tmp, sizeof(char),_read_size,fp);
    dst.insert(dst.size(),tmp,_read_len);
    _read_size-=_read_len;
    _read_all+=_read_len;
    return _read_all;
}

size_t FileSocket::write(std::string &dst,size_t size){
    return 0;
}
}

