#pragma once
#ifndef VERSION_H
#define VERSION_H
#include "Platform.hxx"

#define ABXHTTPD_INT_DEF(X) extern int _##X;
#define ABXHTTPD_INT_GEN(X) int _##X=X;
#define ABXHTTPD_STR_DEF(X) extern char _##X[];
#define ABXHTTPD_STR_GEN(X) char _##X[]=X;

#define ABXHTTPD_VERSION_MAIN 2
#define ABXHTTPD_VERSION_SUB 0
#define ABXHTTPD_VERSION_LAST 3
#define ABXHTTPD_RELEASE_DATE "2022-11-01"

#define ABXHTTPD_STR1(R) #R
#define ABXHTTPD_STR(R) ABXHTTPD_STR1(R)

#define ABXHTTPD_VERSION_HEX 0x0+(ABXHTTPD_VERSION_MAIN << 3) + (ABXHTTPD_VERSION_SUB << 2) + (ABXHTTPD_VERSION_LAST << 1)

#define ABXHTTPD_VERSION_TEXT ABXHTTPD_STR(ABXHTTPD_VERSION_MAIN) "." ABXHTTPD_STR(ABXHTTPD_VERSION_SUB) "." \
ABXHTTPD_STR(ABXHTTPD_VERSION_LAST)

#define ABXHTTPD_VERSION_SERVER "abxhttpd/" ABXHTTPD_VERSION_TEXT

#ifdef __clang__
#define ABXHTTPD_CLANG
#define ABXHTTPD_COMPILER __VERSION__
#elif (defined __GNUC__)
#define ABXHTTPD_COMPILER "GCC " __VERSION__
#define ABXHTTPD_GCC
#elif (defined _MSC_VER)
#define ABXHTTPD_COMPILER "Microsoft Visual C++ ( _MSC_FULL_VER " ABXHTTPD_STR(_MSC_FULL_VER) " )"
#define ABXHTTPD_MSVC
#define ssize_t long;
#else
#define ABXHTTPD_COMPILER __VERSION__
#endif

#define ABXHTTPD_COMPILE_TIME  __DATE__ " " __TIME__

ABXHTTPD_INT_DEF(ABXHTTPD_VERSION_MAIN)
ABXHTTPD_INT_DEF(ABXHTTPD_VERSION_SUB)
ABXHTTPD_INT_DEF(ABXHTTPD_VERSION_LAST)
ABXHTTPD_STR_DEF(ABXHTTPD_RELEASE_DATE)
ABXHTTPD_INT_DEF(ABXHTTPD_VERSION_HEX)
ABXHTTPD_STR_DEF(ABXHTTPD_VERSION_TEXT)
ABXHTTPD_STR_DEF(ABXHTTPD_COMPILE_TIME)

#define ABXHTTPD_INFO "Abxhttpd version: " ABXHTTPD_VERSION_SERVER "\n" \
"Built by " ABXHTTPD_COMPILER "\n" \
"Build Time: " ABXHTTPD_COMPILE_TIME "\n" \
"Copyright (C) 2022 Abxhttpd Group."

#define ABXHTTPD_HELP "Abxhttpd Manual\n\nUsage: [option] ... [ -p port | -D dir | -] [arg] ...\n\n" \
"Options and arguments:\n" \
"-V\t: See version and copyright information.\n" \
"-v\t: Set detailed information grade.\n\tDetailed info of given grade will be printed to STDOUT.\n\tAdd c before grade can print colorful info\n\t(Example: -v 5/-vc11)\n" \
"-m\t: Show cores and modules installed.\n" \
"-c\t: Specify abxhttpd-core to use.\n\t(Example: -c OtherCore, some command line arguments may be different in this case)\n" \
"-C\t: Dynamically load other core or module.(Unrealized)\n\t(Example: -C ./libothercore.so)\n" \
"-h\t: Show manual page(This page).\n" \
"-g\t: Start the graphical user interface(GUI).\n" \
"-p\t: Specify and listening port.\n\t(Example: -p 80)\n" \
"-d\t: Start deamon.\n\t(Example: -d)\n" \
"-b\t: Specify bind IP(Only support TCP4 now).\n\t(Example: -b 127.0.0.1(Default:0.0.0.0))\n" \
"-i\t: Specify the config file.\n\t(Example: -i httpd.ini)(Unrealized)\n" \
"-l\t: Specify normal log files. Default to standard output(1).\n\t(Example: -l access.log)\n" \
"-e\t: Specify error log files. Default to standard error(2).\n\t(Example: -e error.log)\n" \
"-D\t: Set working directory.\n\t(Default: .(Current directory) Example: -D C:\\Users\\Administrator)\n" \
"-T\t: Enable multi-thread mode.\n\t(Default: disabled)\n" 

#endif