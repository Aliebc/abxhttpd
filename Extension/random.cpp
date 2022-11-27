#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "include/Module.hxx"
#include "include/random.H"

namespace abxhttpd {
Random::~Random()=default;
Random::Random()=default;
bool Random::urandom=false;
FILE * Random::urandom_fp=NULL;
size_t Random::count=0;
const char * Random::random_s="Random API";
void * Random::Init(void * pp){
    count=0;
    FILE * fp=fopen("/dev/urandom","rb");
    if(fp==NULL){
        urandom=false;
    }else{
        urandom=true;
        urandom_fp=fp;
    }
    random_s=(urandom?"/dev/urandom (UNIX API)":"rand (C API)");
    ModuleConfigure * pp2=(ModuleConfigure *)pp;
    pp2->Conf[2].info=Random::random_s;
    return NULL;
}

void Random::Generate(void *dst, int len){
    ++count;
    if(urandom){
        fread(dst, 1, len, urandom_fp);
    }else{
        srand(count+time(NULL));
        for(int i=0;i<len;++i){
            *((char*)dst+i)=(char)rand();
        }
    }
}

ModuleConfigure abx_random_info={"random",{
    {"Support","enabled"},
    {"Distribution","uniform"},
    {"Generator",Random::random_s}
},Random::Init,&abx_random_info};

Module abx_random_module(&abx_random_info);

};