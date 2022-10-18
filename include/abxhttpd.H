#pragma once
#ifndef _ABXHTTPD_H_
#define _ABXHTTPD_H_

/*
    ___     __           __     __   __              __
   /   |   / /_   _  __ / /_   / /_ / /_ ____   ____/ /
  / /| |  / __ \ | |/_// __ \ / __// __// __ \ / __  / 
 / ___ | / /_/ /_>  < / / / // /_ / /_ / /_/ // /_/ /  
/_/  |_|/_.___//_/|_|/_/ /_/ \__/ \__// .___/ \__,_/   
                                     /_/               
This is ABXHTTPD main header file, including every sub module's header file.
In your program, you can only include this file to get whole functions.

*/

#ifdef DEBUG
#define ABXHTTPD_DEBUG
#endif

#include "Version.hxx"
#include "abxerror.hxx"
#include "config.hxx"
#include "Module.hxx"
#include "mimes.hxx"
#include "HttpCode.hxx"
#include "HttpRequest.hxx"
#include "HttpResponse.hxx"
#include "Platform.hxx"
#include "Httpd.hxx"
#include "AbxInfo.hxx"
#include "HttpdSocket.hxx"
#include "HttpdTools.hxx"
#include <iostream>
#include <initializer_list>
#include <fstream>

extern char _errorstr[];

#define ABXHTTPD_CLI_ERR(fmt,...) \
if(info_color==0){printf("[Abxhttpd ERROR] " fmt "\n",##__VA_ARGS__);exit(1);}\
else{printf("\033[1;31m[Abxhttpd ERROR] \033[1;37m" fmt "\033[0m\n",##__VA_ARGS__);exit(1);}

#ifdef ABXHTTPD_DEBUG
#ifdef ABXHTTPD_NOCOLOR
#define ABXHTTPD_DEBUG_PRINTF(fmt,...) printf("[Abxhttpd DEBUG][F:" __FILE__ " L:%d]" fmt ,__LINE__,##__VA_ARGS__)
#else
#define ABXHTTPD_DEBUG_PRINTF(fmt,...) printf("\033[1;31m[Abxhttpd DEBUG]\033[0m\033[1;32m[F:" __FILE__ " L:%d] \033[1;37m" fmt "\033[0m",__LINE__,##__VA_ARGS__)
#endif
#else 
#define ABXHTTPD_DEBUG_PRINTF(fmt,...) 
#endif

#define ABXHTTPD_CLI_ERR2(cond,fmt,...) if(!(cond)){ABXHTTPD_CLI_ERR(fmt,##__VA_ARGS__);}

#define ABXHTTPD_ZERO_MEMORY(dst) memset(dst,0,sizeof(dst))

#ifndef ABXHTTPD_NO_INFO
#define ABXHTTPD_INFO_PRINT(grade,fmt,...) if(grade<=verbose){\
if(info_color==0){\
    printf("[Abxhttpd INFO][Grade %d] " fmt "\n",grade,##__VA_ARGS__);\
}else{printf("\033[1;35m[Abxhttpd INFO]\033[1;32m[Grade %d] \033[1;37m" fmt "\033[0m\n",grade,##__VA_ARGS__);fflush(stdout);}\
}
#else
#define ABXHTTPD_INFO_PRINT(grade,fmt,...)
#endif

#endif