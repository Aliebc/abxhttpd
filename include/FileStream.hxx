#ifndef FILE_SOCKET_H
#define FILE_SOCKET_H

#include <stdio.h>
#include "HttpdSocket.hxx"

#define ABXHTTPD_FILE_STDIN 0
#define ABXHTTPD_FILE_STDOUT 1
#define ABXHTTPD_FILE_STDERR 2

namespace abxhttpd {
/**
 * @brief 文件流类
 *
 * 文件流类，封装了C API对文件的操作。
 * 不采用C++的文件流是因为C++的fstream无法和socket API很好的同步
 */
class FileStream:public BasicStream{
private:
    /// 内部文件指针
    FILE * fp;
    /// 获取内部文件长度
    void get_file_length();
    /// 文件长度
    size_t length;
protected:
    /// 打开标志符合集
    static const char * open_method[];
public:
    /// 文件流标志符
    enum FILE_FLAG{
        READ=(1<<0), ///< 读标志符
        WRITE=(1<<1), ///< 写标志符
        BINARY=(1<<2), ///< 二进制标志符
        ADD=(1<<3|WRITE), ///< 追加标志符(必可写)
        STDIN = 0, ///< 标准输入标志符
        STDOUT = 1, ///< 标准输出标志符
        STDERR = 2, ///< 标准错误标志符
    };
    ///默认构造函数
    FileStream();
    /**
     * @brief 构造函数（路径+方法）
     *
     * @param[in] path 文件路径
     * @param[in] method 打开方法
     * 打开方法参见FILE_FLAG，通过与运算传值，如READ|WRITE表示可读写
     * @warning 如果文件无法打开，会抛出异常abxhttpd_error
     */
    FileStream(const char * path,int method);
    /**
     * @brief 构造函数（路径）
     *
     * 无标志符默认为二进制读("rb")
     * @param[in] path 文件路径
     * @warning 如果文件无法打开，会抛出异常abxhttpd_error
     */
    FileStream(const char * path);
    /**
     * @brief 构造函数（文件描述符）
     *
     * @param[in] fd 文件描述符（只接受STDIN/STDOUT/STDERR）
     * @warning 如果文件无法打开，会抛出异常abxhttpd_error
     */
    FileStream(int fd);
    size_t read(std::string & dst,size_t size=0) override;
    size_t write(const std::string & dst,size_t size=0) override;
    /**
     * @brief 获取文件长度
     * @return 文件长度
     */
    size_t get_length() const;
    ///析构函数，释放文件指针
    ~FileStream();
};
}

#endif
