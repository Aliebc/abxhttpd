#ifndef RANDOM_H
#define RANDOM_H
#include <cstdio>
#include <cstdint>

namespace abxhttpd {
class Random{
private:
    static bool urandom;
    static size_t count;
    static FILE * urandom_fp;
public:
    static void Generate(void * dst,int len);
    static const char * random_s;
    static void * Init(void *);
    Random();
    ~Random();
};
}

#endif