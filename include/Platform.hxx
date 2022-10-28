#ifndef _PLATFORM_H
#define _PLATFORM_H

#if (defined __WIN32__) || (defined WINNT) || (defined _WIN32)  || (defined _WIN64)
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

#if (defined __arm64__) || (defined _M_ARM) || (defined _M_ARM64)
#define ABXHTTPD_ARCH "arm64"
#else
#ifdef __aarch64__
#define ABXHTTPD_ARCH "aarch64"
#endif
#endif

#ifdef __x86_64__
#define ABXHTTPD_ARCH "x86_64"
#else
#if (defined __amd64__) || (defined _M_AMD64)
#define ABXHTTPD_ARCH "amd64"
#endif
#endif

#if (defined __i386__) || (defined __x86__) || (defined _M_IX86)
#define ABXHTTPD_ARCH "x86"
#endif

#ifndef ABXHTTPD_ARCH
#define ABXHTTPD_ARCH "Unknown"
#endif

#ifdef ABXHTTPD_UNIX

#define ABXHTTPD_API

#else
#ifdef xhttpd_EXPORTS
#define ABXHTTPD_EXPORT
#endif

#ifdef ABXHTTPD_CLI
#define ABXHTTPD_API
#else
#ifdef ABXHTTPD_EXPORT
#define ABXHTTPD_API __declspec(dllexport)
#else
#define ABXHTTPD_API __declspec(dllimport)
#endif
#endif
#endif

#ifdef ABXHTTPD_UNIX
#include <sys/stat.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/utsname.h>
#endif

#ifdef ABXHTTPD_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#define poll(x,y,z) WSAPoll(x,y,z)
#endif

#endif
