#pragma once
#ifndef _ABXHTTPD_H_
#define _ABXHTTPD_H_

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
#include <iostream>
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

#endif