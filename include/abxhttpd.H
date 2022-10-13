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
#include <iostream>
#include <fstream>

extern char _errorstr[];

#define ABXHTTPD_CLI_ERR(fmt,...) sprintf(_errorstr,"[Abxhttpd Error] " fmt,##__VA_ARGS__);\
std::cout<<_errorstr<<std::endl;\
exit(1)

#define ABXHTTPD_CLI_ERR2(cond,fmt,...) if(!(cond)){ABXHTTPD_CLI_ERR(fmt,##__VA_ARGS__);}

#endif