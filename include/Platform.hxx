#ifndef _PLATFORM_H
#define _PLATFORM_H

#if (defined __WIN32__) || (defined WINNT) || (defined WIN32)
#define ABXHTTPD_PLATFORM "Windows"
#define ABXHTTPD_WINDOWS
#endif

#ifdef __MACH__
#define ABXHTTPD_PLATFORM "MacOS"
#define ABXHTTPD_DARWIN
#define ABXHTTPD_UNIX
#endif

#if (defined linux) || (defined __linux)
#ifdef __ANDROID__
#define ABXHTTPD_PLATFORM "Android"
#define ABXHTTPD_ANDROID
#else
#define ABXHTTPD_PLATFORM "Linux"
#endif
#define ABXHTTPD_LINUX
#define ABXHTTPD_UNIX
#endif

#ifndef ABXHTTPD_PLATFORM
#error Unknown system, failed to continue compiling, please modify include/Platform.hxx.
#define ABXHTTPD_PLATFORM "Unknown"
#endif

#ifdef __arm64__
#define ABXHTTPD_ARCH "arm64"
#else
#ifdef __aarch64__
#define ABXHTTPD_ARCH "aarch64"
#endif
#endif

#ifdef __x86_64__
#define ABXHTTPD_ARCH "x86_64"
#else
#ifdef __amd64__
#define ABXHTTPD_ARCH "amd64"
#endif
#endif

#ifdef __i386__
#define ABXHTTPD_ARCH "i386"
#endif

#ifndef ABXHTTPD_ARCH
#define ABXHTTPD_ARCH "Unknown"
#endif

#ifdef ABXHTTPD_UNIX

#include <unistd.h>
#include <sys/utsname.h>

#endif
#endif