#include "include/HttpdTools.hxx"
#include "include/AbxInfo.hxx"
#include "include/config.hxx"
#include "include/Module.hxx"
#include <ctime>

namespace abxhttpd{

CmdArray CmdParse(int argc, const char ** argv){
    int _p=0;
    CmdArray ret;
    while(true){
        if(*argv[_p]=='-'){
            if(strlen(argv[_p])==2){
                if(_p==argc-1){
                    ret[*(argv[_p]+1)]=std::string();
                    break;
                }else{
                    if(*argv[_p+1]=='-'){
                        ret[*(argv[_p]+1)]=std::string();
                    }else{
                        ret[*(argv[_p]+1)]=argv[_p+1];
                        _p++;
                    }
                }
            }else if(strlen(argv[_p])>2){
                ret[*(argv[_p]+1)]=argv[_p]+2;
            }
        }
        if(_p==argc-1){
            break;
        }
        _p++;
    }
    return ret;
}

HttpdTools::HttpdTools()=default;
HttpdTools::~HttpdTools()=default;

size_t HttpdTools::_FileLength(std::string & _Path){
#ifdef ABXHTTPD_UNIX
    struct stat _ft;
    int st=stat(_Path.c_str(),&_ft);
    if(st!=0){
        throw HttpException(404,"File cannot be opened");
    }
    if(!S_ISREG(_ft.st_mode)){
        throw HttpException(403,"Not Regular File");
    }
    return _ft.st_size;
#else
    FILE * fp=fopen(_Path.c_str(),"rb");
    if(fp==NULL){
        throw HttpException(404,"File cannot be opened");
    }
    size_t f_size;
    fseek(fp,0,SEEK_END);
    f_size=ftell(fp);
    fclose(fp);
    return f_size;
#endif
}

std::string HttpdTools::_FileSuffix(const std::string & _Name){
    size_t _Dot=_Name.find_last_of('.');
    size_t _Len=_Name.size();
    if(std::string::npos==_Dot){
        return std::string();
    }
    return _Name.substr(_Dot,_Len-_Dot);
}

std::string _ABX_URLDecode(const std::string & _src){
    std::string res;
    auto src=_src.begin();
    while(src!=_src.end()){
        if(*(src)=='+'){
            res.append(1,' ');
            src++;
            continue;
        }
        if(*(src)!='%'){
            res.append(1,*src);
            src++;
            continue;
        }else{
            int a;
            sscanf(_src.c_str()+(src-_src.begin())+1,"%2X",&a);
            a-=1<<9;
            res.append(1,static_cast<char>(a));
            src+=3;
        }
    }
    return res;
}

#ifdef ABXHTTPD_WINDOWS

//
char* Utf8ToGB2312(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    
    if (wstr) {
        delete[] wstr;
        wstr = NULL;
    }
    return str;
}

std::string HttpdTools::ABX_URLDecode(const std::string & _src){
    std::string utf_8_c(_ABX_URLDecode(_src));
    char * gbk_c=Utf8ToGB2312(utf_8_c.c_str());
    std::string res(gbk_c);
    delete[] gbk_c;
    return res;
}
#else
std::string HttpdTools::ABX_URLDecode(const std::string & _src){
    return _ABX_URLDecode(_src);
}
#endif

std::string HttpdTools::ABXInfoPageHTML(const HttpRequest * _req){
    std::stringstream res;
    res<<ABXHTTPD_INFO_PAGE_1<<Module::ShowModules_HTML(_req)<<ABXHTTPD_INFO_PAGE_2;
    return res.str();
}

void HttpdTools::ParseQueryString(SSMap & ssm, const std::string & _src) noexcept{
    std::string tmp=_ABX_URLDecode(_src);
    while(tmp.size()>0){
        auto p1=tmp.find_first_of('=');
        if(p1>tmp.size()){
            break;
        }
        std::string key=tmp.substr(0,p1);
        tmp.erase(0,p1+1);
        auto p2=tmp.find_first_of('&');
        std::string val=tmp.substr(0,p2);
        if(p2>tmp.size()){
            ssm[key]=tmp;
        }else{
            tmp.erase(0,p2+1);
            ssm[key]=val;
        }
    }
}

void HttpdTools::ParseCookie(SSMap & ssm, const std::string & _src) noexcept{
    std::string tmp(_src);
    while(tmp.size()>0){
        auto p1=tmp.find_first_of('=');
        if(p1>tmp.size()){
            break;
        }
        std::string key=tmp.substr(0,p1);
        tmp.erase(0,p1+1);
        auto p2=tmp.find_first_of(';');
        std::string val=tmp.substr(0,p2);
        if(p2>tmp.size()){
            ssm[key]=tmp;
        }else{
            tmp.erase(0,p2+1);
            ssm[key]=val;
            auto p3=tmp.find_first_not_of(' ');
            tmp.erase(0,p3);
        }
    }
}

}
