#include "include/HttpdSocket.hxx"
#include "include/FileStream.hxx"

namespace abxhttpd{
FileStream::FileStream(const char * path):BasicStream(){
    fp=fopen(path,"rb");
    if(fp==NULL){
        throw abxhttpd_error("Not Regular File");
    }
    status_id|=(ABXHTTPD_STREAM_READABLE|ABXHTTPD_STREAM_WRITEABLE);
    get_file_length();
    clear_tmp();
}
FileStream::~FileStream(){
    status_id=ABXHTTPD_STREAM_CLOSED;
    fclose(fp);
}
void FileStream::get_file_length(){
    fseek(fp, 0, SEEK_END);
    length=ftell(fp);
    rewind(fp);
}
size_t FileStream::get_length() const{
    return length;
}
size_t FileStream::read(std::string &dst,size_t size){
    if(size==0){size--;}
    size_t _read_size=ABXHTTPD_MIN(size, length);
    size_t _read_len=0;
    size_t _read_all=0;
    while(_read_size>0&&(!feof(fp))){
        clear_tmp();
        _read_len=fread(buffer_tmp, sizeof(char), buffer_size(), fp);
        dst.insert(dst.size(), buffer_tmp, _read_len);
        _read_size-=_read_len;
        _read_all+=_read_len;
    }
    if(feof(fp)){
        status_id&=(~ABXHTTPD_STREAM_READABLE);
    }
    return _read_all;
}

size_t FileStream::write(const std::string &dst,size_t size){
    return 0;
}

}

