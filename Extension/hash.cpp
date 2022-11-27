#include "include/hash.H"
#include "include/Module.hxx"
#include "openssl/opensslv.h"
#include <cstdio>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <utility>
#include <memory>

namespace abxhttpd {

ModuleConfigure abx_hash_info={"hash",{
    {"Support","enabled"},
    {"Crypto Library Version",OPENSSL_VERSION_TEXT},
    {"Method","md5,sha256,sha384,sha512"}
}};

Module abx_hash_module(&abx_hash_info);

std::string Hash::md5(const std::string && src){
    char tmp[33]={};
    unsigned char * tmpuc=new unsigned char[16];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, src.c_str(), src.size());
    MD5_Final(tmpuc, &ctx);
    for(int _i=0;_i<15;_i++){
        snprintf(tmp+_i*2, 3, "%02x", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::MD5(const std::string && src){
    char tmp[33]={};
    unsigned char * tmpuc=new unsigned char[16];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, src.c_str(), src.size());
    MD5_Final(tmpuc, &ctx);
    for(int _i=0;_i<16;_i++){
        snprintf(tmp+_i*2, 3, "%02X", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::md5(const std::string & src){
    return md5(std::move(src));
}

std::string Hash::MD5(const std::string & src){
    return MD5(std::move(src));
}

std::string Hash::sha256(const std::string && src){
    char tmp[65]={};
    unsigned char * tmpuc=new unsigned char[32];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, src.c_str(), src.size());
    SHA256_Final(tmpuc, &ctx);
    for(int _i=0;_i<32;_i++){
        snprintf(tmp+_i*2, 3, "%02x", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::SHA256(const std::string && src){
    char tmp[65]={};
    unsigned char * tmpuc=new unsigned char[32];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, src.c_str(), src.size());
    SHA256_Final(tmpuc, &ctx);
    for(int _i=0;_i<32;_i++){
        snprintf(tmp+_i*2, 3, "%02X", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}


std::string Hash::sha256(const std::string & src){
    return sha256(std::move(src));
}

std::string Hash::SHA256(const std::string & src){
    return SHA256(std::move(src));
}

std::string Hash::sha384(const std::string && src){
    char tmp[97]={};
    unsigned char * tmpuc=new unsigned char[48];
    SHA512_CTX ctx;
    SHA384_Init(&ctx);
    SHA384_Update(&ctx, src.c_str(), src.size());
    SHA384_Final(tmpuc, &ctx);
    for(int _i=0;_i<48;_i++){
        snprintf(tmp+_i*2, 3, "%02x", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::SHA384(const std::string && src){
    char tmp[97]={};
    unsigned char * tmpuc=new unsigned char[48];
    SHA512_CTX ctx;
    SHA384_Init(&ctx);
    SHA384_Update(&ctx, src.c_str(), src.size());
    SHA384_Final(tmpuc, &ctx);
    for(int _i=0;_i<48;_i++){
        snprintf(tmp+_i*2, 3, "%02X", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::sha384(const std::string & src){
    return sha384(std::move(src));
}

std::string Hash::SHA384(const std::string & src){
    return SHA384(std::move(src));
}

std::string Hash::sha512(const std::string && src){
    char tmp[97]={};
    unsigned char * tmpuc=new unsigned char[48];
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, src.c_str(), src.size());
    SHA512_Final(tmpuc, &ctx);
    for(int _i=0;_i<48;_i++){
        snprintf(tmp+_i*2, 3, "%02x", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::SHA512(const std::string && src){
    char tmp[97]={};
    unsigned char * tmpuc=new unsigned char[48];
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, src.c_str(), src.size());
    SHA512_Final(tmpuc, &ctx);
    for(int _i=0;_i<48;_i++){
        snprintf(tmp+_i*2, 3, "%02X", tmpuc[_i]);
    }
    delete [] tmpuc;
    return tmp;
}

std::string Hash::sha512(const std::string & src){
    return sha512(std::move(src));
}

std::string Hash::SHA512(const std::string & src){
    return SHA512(std::move(src));
}

Hash::Hash()=default;
Hash::~Hash()=default;

}