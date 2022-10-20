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
abxhttpd::Httpd * CMain=NULL;

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

int main(int argc,char * argv[]){
    global_argc=argc;
    global_argv=(const char **)argv;
    CmdArray cmd_argu=CmdParse(argc,(const char **)argv);
    global_argu=&cmd_argu;
    signal(SIGINT,sigint_handle);
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'v')){
        try{
            if(cmd_argu['v'].at(0)=='c'){
                info_color=1;
                verbose=atoi(cmd_argu['v'].c_str()+1);
            }else{
                verbose=std::stoi(cmd_argu['v']);
            }
        }catch(std::exception &e){
            verbose=0;
        }
    }
    ABXHTTPD_INFO_PRINT(3,"[Main]Parsed command line arguments.");
    const HttpdCore * _HttpdCore;
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'h')){
        std::cout << ABXHTTPD_HELP <<std::endl;
        exit(0);
    }
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'V')){
        std::cout << ABXHTTPD_INFO <<std::endl;
        exit(0);
    }
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'g')){
        #ifdef ABXHTTPD_GUI
        start_gui(NULL);
        #else
        ABXHTTPD_CLI_ERR("GUI Module not installed.");
        #endif
    }
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'m')){
        std::cout << "[Cores]\n"<< ShowHttpdCoreAddressTable() <<std::endl;
        std::cout << "[Modules]\n"<< ShowModules()<<std::endl ;
        exit(0);
    }
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'c')){
        _HttpdCore=FindHttpdCore(cmd_argu['c'].c_str());
        ABXHTTPD_CLI_ERR2(_HttpdCore!=NULL,"Cannot find specified Httpd Core. Please use -m option to see useable core symbol.");
    }else{
        _HttpdCore=&DefaultHttpdCore;
    }
    
    if(CmdArrayIs((const CmdArray *)&cmd_argu,'p')){
        SocketSettingList si={0};
        si.Is_reused=true;
        if(CmdArrayIs((const CmdArray *)&cmd_argu,'b')){
            struct in_addr bind_ip;
            if(inet_pton(AF_INET,cmd_argu['b'].c_str(),&bind_ip)==0){
                ABXHTTPD_CLI_ERR("Cannot Bind IP %s.(Format Error)",cmd_argu['b'].c_str());
            }
            si.Bind_IP=bind_ip.s_addr;
        }
        si.Port=atoi(cmd_argu['p'].c_str());
        si.Max_connect_count=SOMAXCONN;
        HttpdSettingList hi;
        hi.Socket_S=si;
        ThreadSettingList thread_set;
        thread_set.Thread_count=atoi(cmd_argu['T'].c_str());
        thread_set.Multi_thread=CmdArrayIs((const CmdArray *)&cmd_argu,'T');
        std::ofstream out_log;
        std::ofstream out_err;
        thread_set.abxout=&std::cout;
        thread_set.abxerr=&std::cerr;
        if(CmdArrayIs((const CmdArray *)&cmd_argu,'l')){
            out_log.open(cmd_argu['l'],std::ios::app);
            if(out_log.is_open()){
                thread_set.abxout=&out_log;
            }else{
                ABXHTTPD_CLI_ERR("Cannot open file %s.",cmd_argu['l'].c_str());
            }
        }
        if(CmdArrayIs((const CmdArray *)&cmd_argu,'e')){
            out_err.open(cmd_argu['e'],std::ios::app);
            if(out_err.is_open()){
                thread_set.abxerr=&out_err;
            }else{
                ABXHTTPD_CLI_ERR("Cannot open file %s.",cmd_argu['e'].c_str());
            }
        }
        hi.Thread_S=thread_set;
        hi.Http_S.Path=CmdArrayIs((const CmdArray *)&cmd_argu,'D')?cmd_argu['D']:".";
        if(CmdArrayIs((const CmdArray *)&cmd_argu,'d')){
            //char * _name=(char *)malloc(1024);
            if(CmdArrayIs((const CmdArray *)&cmd_argu,'l')&&CmdArrayIs((const CmdArray *)&cmd_argu,'e')){

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
        Httpd httpd_main(*_HttpdCore,hi);
        CMain=&httpd_main;
        try{
            ABXHTTPD_INFO_PRINT(5,"[Main]Start Core at %p.",_HttpdCore);
            httpd_main.start();
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
