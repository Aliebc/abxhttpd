#include "include/abxhttpd.H"
#include <ctime>

namespace abxhttpd{

    CmdArray CmdParse(int argc, const char * argv[]){
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

    size_t _FileLength(std::string _Path){
        FILE * fp=fopen(_Path.c_str(),"rb");
        if(fp==NULL){
            throw abxhttpd_error_http(404);
        }
        #ifdef ABXHTTPD_UNIX
        struct stat _ft;
        stat(_Path.c_str(),&_ft);
        if(!S_ISREG(_ft.st_mode)){
            throw abxhttpd_error_http(403);
            //throw abxhttpd_error("Not Regular File");
        }
        #endif
        size_t f_size;
        fseek(fp,0,SEEK_END);
        f_size=ftell(fp);
        fclose(fp);
        return f_size;
    }

    std::string _FileRead(std::string && _Path){
        std::string fps;
        FILE * fp=fopen(_Path.c_str(),"rb");
        if(fp==NULL){
            throw abxhttpd_error("File not exists");
        }
        #ifdef ABXHTTPD_UNIX
        struct stat _ft;
        stat(_Path.c_str(),&_ft);
        if(!S_ISREG(_ft.st_mode)){
            throw abxhttpd_error("Not Regular File");
        }
        char _pc[4096];
        size_t _rd=0;
        size_t _ed=_ft.st_size;
        size_t _pd=0;
        #else
        DWORD st =GetFileAttributes(_Path.c_str());
        if(st&FILE_ATTRIBUTE_DIRECTORY){
            throw abxhttpd_error("Not Regular File");
        }
        size_t f_size;
        char _pc[4096];
        fseek(fp,0,SEEK_END);
        f_size=ftell(fp);
        size_t _rd=0;
        size_t _ed=f_size;
        size_t _pd=0;
        rewind(fp);
        #endif
        while(true){
            _rd=fread(_pc,sizeof(char),sizeof(_pc),fp);
            if(_pd==_ed){
                break;
            }
            fps+=std::string(_pc,_rd);
            memset(_pc,0,sizeof(_pc));
            _pd+=_rd;
        }
        fclose(fp);
        return fps;
    }

    std::string _FileSuffix(std::string  _Name){
        size_t _Dot=_Name.find_last_of('.');
        size_t _Len=_Name.size();
        if(std::string::npos==_Dot){
            return std::string();
        }
        return _Name.substr(_Dot,_Len-_Dot);
    }

        std::string _ABX_URLDecode(std::string  _src){
        std::string res;
        auto src=_src.begin();
        while(src!=_src.end()){
            if(*(src)!='%'){
                res.append(1,*src);
                src++;
                continue;
            }else{
                int a;
                sscanf(_src.c_str()+(src-_src.begin())+1,"%2X",&a);
                a-=256;
                res.append(1,static_cast<char>(a));
                src+=3;
            }
        }
        return res;
    }

    #ifdef ABXHTTPD_WINDOWS
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
    std::string ABX_URLDecode(std::string & _src){
        std::string utf_8_c(_ABX_URLDecode(_src));
        char * gbk_c=Utf8ToGB2312(utf_8_c.c_str());
        std::string res(gbk_c);
        delete[] gbk_c;
        return res;
    }
    #else
    std::string ABX_URLDecode(std::string & _src){
        return _ABX_URLDecode(_src);
    }
    #endif
}
