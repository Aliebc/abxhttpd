#include "include/FileStream.hxx"
#include <errno.h>
#include <string.h>

#define I_MIN(a,b) (a<b?a:b)

namespace abxhttpd{

FileStream::FileStream(const char * path,int method):BasicStream(){
    length=0;
    if(!(method>0&&method<16)){
        throw BasicException("No method");
    }else{
        if(open_method[method]==NULL){
            throw BasicException("No method");
        }else{
            fp=fopen(path, open_method[method]);
            if(fp==NULL){
                throw BasicException(strerror(errno));
            }
        }
    }
    if(FILE_FLAG::READ&method){
        status_id|=(READABLE);
        get_file_length();
    }
    if(FILE_FLAG::WRITE&method){
        status_id|=(BasicStream::FLAG::WRITEABLE);
        length=0;
    }
    clear_tmp();
}

FileStream::FileStream(const char * path):
FileStream(path,FILE_FLAG::READ|FILE_FLAG::BINARY){}

FileStream::FileStream(int fd){
    length=0;
    switch (fd) {
        case FILE_FLAG::STDIN:
            fp=stdin;
            status_id|=FILE_FLAG::READ;
            break;
        case FILE_FLAG::STDOUT:
            fp=stdout;
            status_id|=FILE_FLAG::WRITE;
            break;
        case FILE_FLAG::STDERR:
            fp=stderr;
            status_id|=FILE_FLAG::WRITE;
            break;
        default:
            throw BasicException("Not a valid fd");
            break;
    }
}

FileStream::~FileStream(){
    status_id=CLOSED;
    fclose(fp);
}

void FileStream::get_file_length(){
    fseek(fp, 0, SEEK_END);
    length=ftell(fp);
    rewind(fp);
}

bool FileStream::SetOffset(size_t offset){
    int op=fseek(fp, offset, SEEK_SET);
    if(op!=0){
        SetLastError(strerror(errno));
        return false;
    }
    return true;
}

size_t FileStream::get_length() const{
    return length;
}

size_t FileStream::read(std::string &dst,size_t size){
    if(!(status_id&READABLE)){
        SetLastError("Cannot read file now");
        return 0;
    }
    if(size==0){size--;}
    size_t _read_size=I_MIN(size, length);
    size_t _read_all=0;
    while(_read_size>0&&(!feof(fp))){
        size_t _read_len=0;
        clear_tmp();
        _read_len=fread(buffer_tmp, sizeof(char), buffer_size(), fp);
        dst.insert(dst.size(), buffer_tmp, _read_len);
        _read_size-=_read_len;
        _read_all+=_read_len;
    }
    return _read_all;
}

size_t FileStream::write(const std::string &dst,size_t size){
    if(!(status_id&BasicStream::FLAG::WRITEABLE)){
        SetLastError("File is read-only");
        return 0;
    }
    if(size==0){size--;}
    size_t _write_size=I_MIN(size, dst.size());
    size_t _write_len=0;
    _write_len+=fwrite(dst.c_str(), sizeof(char), _write_size, fp);
    return _write_len;
}

bool FileStream::close(){
    if(status_id!=CLOSED){
        status_id=CLOSED;
        if(fclose(fp)!=0){
            return false;
        }
    }
    return true;
}

const char * FileStream::open_method[]={
    NULL, // 0
    "r", //I_FLAG::READ 1
    "w", //I_FLAG::WRITE 2
    "r+", //I_FLAG::READ|I_FLAG::WRITE 3
    "b", //I_FLAG::BINARY 4
    "rb", //I_FLAG::BINARY|I_FLAG::READ 5
    "wb", //I_FLAG::BINARY|I_FLAG::WRITE 6
    "rb+", //I_FLAG::BINARY|I_FLAG::READ|I_FLAG::WRITE 7
    NULL, //8
    NULL, //9
    "a", //I_FLAG::ADD 10
    "a+", //I_FLAG::ADD|I_FLAG::READ 11
    NULL, //12
    NULL, //13
    "ab", //I_FLAG::BINARY|I_FLAG::ADD 14
    "ab+" //I_FLAG::BINARY|I_FLAG::ADD|I_FLAG::READ 15
};

}

