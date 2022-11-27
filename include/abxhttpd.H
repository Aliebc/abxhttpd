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
#include "BasicException.hxx"
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

#endif
