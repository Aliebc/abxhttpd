#ifndef ABX_HASH_H
#define ABX_HASH_H
#include <string>
#include "Module.hxx"

namespace abxhttpd {
class ABXHTTPD_API Hash{
public:
    static std::string md5(const std::string &);
    static std::string md5(const std::string &&);
    static std::string MD5(const std::string &);
    static std::string MD5(const std::string &&);
    static std::string sha256(const std::string &);
    static std::string sha256(const std::string &&);
    static std::string SHA256(const std::string &);
    static std::string SHA256(const std::string &&);
    static std::string sha384(const std::string &);
    static std::string sha384(const std::string &&);
    static std::string SHA384(const std::string &);
    static std::string SHA384(const std::string &&);
    static std::string sha512(const std::string &);
    static std::string sha512(const std::string &&);
    static std::string SHA512(const std::string &);
    static std::string SHA512(const std::string &&);
    Hash();
    ~Hash();
};
}

#endif