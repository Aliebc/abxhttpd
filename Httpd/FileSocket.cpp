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
    size_t _read_size=ABXHTTPD_MIN(size, length);
    size_t _read_len=0;
    size_t _read_all=0;
    while(_read_size>0&&(!feof(fp))){
        ABXHTTPD_ZERO_MEMORY(tmp);
        _read_len=fread(tmp, sizeof(char), sizeof(tmp), fp);
        dst.insert(dst.size(), tmp, _read_len);
        _read_size-=_read_len;
        _read_all+=_read_len;
    }
    return _read_all;
}

size_t FileSocket::write(const std::string &dst,size_t size){
    return 0;
}

HttpdSocket & operator<< (HttpdSocket & _dst, FileSocket & _src){
    std::string tmp_s;
    size_t _read_len=0;
    while ((_read_len=_src.read(tmp_s,ABXHTTPD_BUFFER_SIZE))!=0
           &&_dst.status()!=ABXHTTPD_SOCK_CLOSED) {
        _dst.write(tmp_s,_read_len);
        tmp_s.clear();
    }
    return _dst;
}

}

