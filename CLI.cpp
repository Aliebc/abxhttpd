#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>
#include "include/abxhttpd.H"

using namespace abxhttpd;

int main(int argc,char * argv[]){
    std::ios::sync_with_stdio(false);
    CmdArray d=CmdParse(argc,(const char **)argv);
    HttpdCore * _HttpdCore;
    if(CmdArrayIs(d,'h')){
        std::cout << ABXHTTPD_HELP <<std::endl;
        exit(0);
    }
    if(CmdArrayIs(d,'V')){
        std::cout << ABXHTTPD_INFO <<std::endl;
        exit(0);
    }
    if(CmdArrayIs(d,'g')){
        #ifdef ABXHTTPD_GUI
        start_gui(NULL);
        #else
        ABXHTTPD_CLI_ERR("GUI Module not installed.");
        #endif
    }
    if(CmdArrayIs(d,'m')){
        std::cout << "[Cores]\n"<< ShowHttpdCoreAddressTable() <<std::endl;
        std::cout << "[Modules]\n"<< ShowModules() <<std::endl;
        exit(0);
    }
    if(CmdArrayIs(d,'c')){
        _HttpdCore=FindHttpdCore(d['c'].c_str());
        ABXHTTPD_CLI_ERR2(_HttpdCore!=NULL,"Cannot find specified Httpd Core. Please use -m option to see useable core symbol.");
    }else{
        _HttpdCore=&DefaultHttpdCore;
    }
    
    if(CmdArrayIs(d,'p')){
        SocketSettingList si={0};
        si.Is_reused=true;
        if(CmdArrayIs(d,'b')){
            struct in_addr bind_ip;
            if(inet_pton(AF_INET,d['b'].c_str(),&bind_ip)==0){
                ABXHTTPD_CLI_ERR("Cannot Bind IP %s.(Format Error)",d['b'].c_str());
            }
            si.Bind_IP=bind_ip.s_addr;
        }
        si.Port=atoi(d['p'].c_str());
        si.Max_connect_count=1;
        HttpdSettingList hi;
        hi.Socket_S=si;
        ThreadSettingList ti;
        ti.Multi_thread=CmdArrayIs(d,'T');
        std::ofstream out_log;
        std::ofstream out_err;
        ti.abxout=&std::cout;
        ti.abxerr=&std::cerr;
        if(CmdArrayIs(d,'l')){
            out_log.open(d['l'],std::ios::app);
            if(out_log.is_open()){
                ti.abxout=&out_log;
            }else{
                ABXHTTPD_CLI_ERR("Cannot open file %s.",d['l'].c_str());
            }
        }
        if(CmdArrayIs(d,'e')){
            out_err.open(d['e'],std::ios::app);
            if(out_err.is_open()){
                ti.abxerr=&out_err;
            }else{
                ABXHTTPD_CLI_ERR("Cannot open file %s.",d['e'].c_str());
            }
        }
        hi.Thread_S=ti;
        pid_t _pd;
        hi.Http_S.Path=CmdArrayIs(d,'D')?d['D']:std::string(".");
        if(CmdArrayIs(d,'d')){
            //char * _name=(char *)malloc(1024);
            if(CmdArrayIs(d,'l')&&CmdArrayIs(d,'e')){

            }else{
                ABXHTTPD_CLI_ERR("You should specify log file and error log file in deamon mode.");
            }
            #ifdef ABXHTTPD_UNIX
            pid_t _pd;
            _pd=fork();
            if(_pd<0){
                ABXHTTPD_CLI_ERR("Cannot fork new process.");
                exit(0);
            }else{
                if(_pd==0){
                    close(0);
                    close(1);
                    close(2);
                }else{
                    exit(0);
                }
            }
            #endif
            #ifdef ABXHTTPD_WINDOWS
            TCHAR CLA[1024];
            ZeroMemory(CLA,sizeof(CLA));
            char * cp=(char *)&CLA;
            for(int i=0;i<argc;i++){
                if(strcmp("-d",argv[i])==0){
                    continue;
                }
                sprintf(cp,"%s ",argv[i]);
                cp+=strlen(argv[i])+1;
            }
            if(ShellExecuteA(NULL,"open",argv[0],CLA,".",SW_HIDE)){
                ExitProcess(0);
            }else{
                ABXHTTPD_CLI_ERR("Cannot create new process.");
            }
            #endif
        }
        Httpd ma(*_HttpdCore,hi);
        try{
            ma.start();
        }catch (abxhttpd_error e){
            ABXHTTPD_CLI_ERR("Start failed: %s",e.what());
        }
    }else{
        std::cout << ABXHTTPD_HELP <<std::endl;
        std::cout << "Press Enter key to exit..." <<std::endl;
        getchar();
        exit(0);
    }
    return 0;
}