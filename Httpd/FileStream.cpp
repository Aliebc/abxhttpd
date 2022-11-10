#include "include/HttpdSocket.hxx"
#include "include/FileStream.hxx"
#include <errno.h>
#include <string.h>

namespace abxhttpd{
FileStream::FileStream(const char * path,int method):BasicStream(){
    if(!(method>0&&method<16)){
        throw abxhttpd_error("No method");
    }else{
        if(open_method[method]==NULL){
            throw abxhttpd_error("No method");
        }else{
            fp=fopen(path, open_method[method]);
            if(fp==NULL){
                throw abxhttpd_error(strerror(errno));
            }
        }
    }
    if(ABXHTTPD_FILE_READ&method){
        status_id|=(ABXHTTPD_STREAM_READABLE);
        get_file_length();
    }
    if(ABXHTTPD_FILE_WRITE&method){
        status_id|=(ABXHTTPD_STREAM_WRITEABLE);
        length=0;
    }
    clear_tmp();
}

FileStream::FileStream(const char * path):
FileStream(path,ABXHTTPD_FILE_READ|ABXHTTPD_FILE_BINARY){}

FileStream::FileStream(int fd){
    switch (fd) {
        case ABXHTTPD_FILE_STDIN:
            fp=stdin;
            status_id|=ABXHTTPD_FILE_READ;
            break;
        case ABXHTTPD_FILE_STDOUT:
            fp=stdout;
            status_id|=ABXHTTPD_FILE_WRITE;
            break;
        case ABXHTTPD_FILE_STDERR:
            fp=stderr;
            status_id|=ABXHTTPD_FILE_WRITE;
            break;
        default:
            throw abxhttpd_error("Not a valid fd");
            break;
    }
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
    if(!(status_id&ABXHTTPD_STREAM_READABLE)){
        SetLastError("Cannot read file now");
        return 0;
    }
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
    return _read_all;
}

size_t FileStream::write(const std::string &dst,size_t size){
    if(!(status_id&ABXHTTPD_STREAM_WRITEABLE)){
        SetLastError("File is read-only");
        return 0;
    }
    if(size==0){size--;}
    size_t _write_size=ABXHTTPD_MIN(size, dst.size());
    size_t _write_len=0;
    _write_len+=fwrite(dst.c_str(), sizeof(char), _write_size, fp);
    return _write_len;
}

const char * FileStream::open_method[]={
    NULL, // 0
    "r", //ABXHTTPD_FILE_READ 1
    "w", //ABXHTTPD_FILE_WRITE 2
    "r+", //ABXHTTPD_FILE_READ|ABXHTTPD_FILE_WRITE 3
    "b", //ABXHTTPD_FILE_BINARY 4
    "rb", //ABXHTTPD_FILE_BINARY|ABXHTTPD_FILE_READ 5
    "wb", //ABXHTTPD_FILE_BINARY|ABXHTTPD_FILE_WRITE 6
    "rb+", //ABXHTTPD_FILE_BINARY|ABXHTTPD_FILE_READ|ABXHTTPD_FILE_WRITE 7
    NULL, //8
    NULL, //9
    "a", //ABXHTTPD_FILE_ADD 10
    "a+", //ABXHTTPD_FILE_ADD|ABXHTTPD_FILE_READ 11
    NULL, //12
    NULL, //13
    "ab", //ABXHTTPD_FILE_BINARY|ABXHTTPD_FILE_ADD 14
    "ab+" //ABXHTTPD_FILE_BINARY|ABXHTTPD_FILE_ADD|ABXHTTPD_FILE_READ 15
};

FileStream fs_stdin(ABXHTTPD_FILE_STDIN);
FileStream fs_stdout(ABXHTTPD_FILE_STDOUT);
FileStream fs_stderr(ABXHTTPD_FILE_STDERR);

}

