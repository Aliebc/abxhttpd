#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <map>
#include <signal.h>
#include "include/abxhttpd.H"

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace abxhttpd;
Httpd * CMain=NULL;
void sigint_handle(int);
void abxhttpd_fork(int argc,const char ** argv);

int main(int argc,char ** argv){
    signal(SIGINT,sigint_handle);
    Httpd::global_argc=argc;
    Httpd::global_argv=(const char **)argv;
    CmdArray cmd_argu=CmdParse(argc,(const char **)argv);
    Httpd::global_argu=&cmd_argu;
    HttpdSettingList MainSetting{};
    if(CmdArrayIs(&cmd_argu,'v')){
        if(cmd_argu['v'].at(0)=='c'){
            Httpd::info_color=1;
            Httpd::verbose=atoi(cmd_argu['v'].c_str()+1);
        }else{
            Httpd::verbose=atoi(cmd_argu['v'].c_str());
        }
    }
    const HttpdCore * _HttpdCore;
    if(CmdArrayIs(&cmd_argu,'h')){
        std::cout << ABXHTTPD_HELP <<std::endl;
        exit(0);
    }
    if(CmdArrayIs(&cmd_argu,'V')){
        std::cout << ABXHTTPD_INFO <<std::endl;
        exit(0);
    }
    if(CmdArrayIs(&cmd_argu,'g')){
        #ifdef ABXHTTPD_GUI
        start_gui(NULL);
        #else
        ABXHTTPD_CLI_ERR("GUI Module not installed.");
        #endif
    }
    if(CmdArrayIs(&cmd_argu,'m')){
        std::cout << "[Cores]\n"<< ShowHttpdCoreAddressTable() <<std::endl << std::endl;
        std::cout << "[Modules]\n"<< Module::ShowModules()<<std::endl ;
        exit(0);
    }
    if(CmdArrayIs(&cmd_argu,'c')){
        _HttpdCore=FindHttpdCore(cmd_argu['c'].c_str());
        ABXHTTPD_CLI_ERR2(_HttpdCore!=NULL,"Cannot find specified Httpd Core. Please use -m option to see useable core symbol.");
    }else{
        _HttpdCore=&DefaultHttpdCore;
    }
    
    if(CmdArrayIs(&cmd_argu,'p')){
        if(CmdArrayIs(&cmd_argu,'b')){
            if(CmdArrayIs(&cmd_argu,'6')){
                MainSetting.Socket_S.IPVer=6;
            }else{
                MainSetting.Socket_S.IPVer=4;
            }
            MainSetting.Socket_S.IPStr=cmd_argu['b'].c_str();
        }else{
            MainSetting.Socket_S.IPVer=4;
            MainSetting.Socket_S.IPStr="0.0.0.0";
        }
        MainSetting.Socket_S.Port=atoi(cmd_argu['p'].c_str())%65535;
        MainSetting.Socket_S.MaxConnect=ABXHTTPD_CONNECT_MAX;
        if(CmdArrayIs(&cmd_argu,'T')){
            MainSetting.Thread_S.Multithreading=true;
            MainSetting.Thread_S.ThreadCount=atoi(cmd_argu['T'].c_str());
        }
        if(CmdArrayIs(&cmd_argu,'l')){
            MainSetting.Thread_S.Log=cmd_argu['l'].c_str();
        }
        if(CmdArrayIs(&cmd_argu,'e')){
            MainSetting.Thread_S.Error=cmd_argu['e'].c_str();
        }
        MainSetting.Http_S.Path=CmdArrayIs(&cmd_argu,'D')?cmd_argu['D'].c_str():".";
        if(CmdArrayIs(&cmd_argu,'d')){
            if(!(CmdArrayIs(&cmd_argu,'l')&&CmdArrayIs(&cmd_argu,'e'))){
                ABXHTTPD_CLI_ERR("You should specify log file and error log file in deamon mode.");
            }
            abxhttpd_fork(argc,(const char **)argv);
        }
        if(cmd_argu['c']=="SSLHttpdCore"){
            Httpd::SetExtraSetting("SSL","TRUE");
            Httpd::SetExtraSetting("SSL_CRETIFICATE", cmd_argu['K'].c_str());
            Httpd::SetExtraSetting("SSL_KEY",cmd_argu['k'].c_str());
        }
        try{
            Httpd httpd_main(*_HttpdCore,MainSetting);
            CMain=&httpd_main;
            ABXHTTPD_INFO_PRINT(5,"[Main]Start Core at %p.",_HttpdCore);
            httpd_main.start();
        }catch (const BasicException & e){
            ABXHTTPD_CLI_ERR("Caught Exception: %s",e.what());
        }
    }else{
        std::cout << ABXHTTPD_HELP <<std::endl;
        std::cout << "Press Enter key to exit..." <<std::endl;
        getchar();
        exit(0);
    }
    return 0;
}


void sigint_handle(int sig){
    if(sig==SIGINT){
        ABXHTTPD_INFO_PRINT(1,"[Main]Recieved SIGINT, shutting down.");
        if(CMain!=NULL){
            httpd_t st=CMain->stop();
            ABXHTTPD_INFO_PRINT(1,"[Main]Exit with %d.",st);
            exit(st);
        }
    }
}

void abxhttpd_fork(int argc,const char ** argv){
    #ifdef ABXHTTPD_UNIX
    pid_t _pd;
    _pd=fork();
    if(_pd<0){
        ABXHTTPD_CLI_ERR("Cannot fork new process.");
        exit(0);
    }else{
        if(_pd==0){
            /*close(0);
            close(1);
            close(2);*/
        }else{
            exit(0);
        }
    }
    #endif
    #ifdef ABXHTTPD_WINDOWS
    TCHAR CLA[1024];
    ZeroMemory(CLA,sizeof(CLA));
    char * cp=(char *)&CLA;
    for(int i=1;i<argc;i++){
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
