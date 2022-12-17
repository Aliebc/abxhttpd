#ifndef RANDOM_H
#define RANDOM_H
#include <cstdio>
#include <cstdint>

namespace abxhttpd {
/**
 * @brief 随机数拓展模块
 *
 * 在Windows上采用C API生成传统随机数,在Linux/MacOS上使用设备/dev/urandom随机数发生器
 */
class Random{
private:
    static bool urandom; ///< 是否支持/dev/urandom
    static size_t count; ///< 内部计数器
    static FILE * urandom_fp; ///< /dev/urandom文件指针
public:
    /**
    * @brief 生成随机数
    * 
    * 将使用长度为len的随机数填充字节区dst
    * @param dst[out] 接受随机数的缓冲区
    * @param len[in] 长度
    */
    static void Generate(void * dst,int len); 
    static const char * random_s; ///< 随机数发生器
    static void * Init(void *); ///< 初始化随机数发生器
    /**
     * @brief 默认构造函数
     * 
     * 无需构造,直接调用即可
     * @code 
     * int k;Random::Generate(&k,sizeof(k));
     * cout << k << endl;
     * @endcode
     * 
     */
    Random();
    ~Random();
};
}

#endif