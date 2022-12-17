#ifndef ABX_HASH_H
#define ABX_HASH_H
#include <string>
#include "Module.hxx"

namespace abxhttpd {
/**
 * @brief 哈希拓展模块
 * 
 * 提供主流的md5,sha256,sha384,sha512(包含大小写)哈希运算拓展
 */
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