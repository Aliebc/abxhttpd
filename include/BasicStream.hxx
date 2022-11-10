#ifndef BASICSTREAM_H
#define BASICSTREAM_H
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

namespace abxhttpd{

#define ABXHTTPD_STREAM_BUFFER 1024

/**
 * @brief 流基类
 *
 * 所有流的基类，由此可以派生出套接字流、文件流、字符串流、日志流等。
 * 其核心在于维护了一个缓冲区，并封装了缓冲区和IO相关的操作
 */

class BasicStream{
private:
    static const size_t __buffer_size=ABXHTTPD_STREAM_BUFFER; ///< 流缓冲区大小
protected:
    /**
    @brief 状态
     */
    int status_id;
    /**
    @brief 缓冲区地址
     */
    char * buffer_tmp;
    /**
    @brief 清空缓冲区内存
    @return 无
     */
    void clear_tmp();
    /**
    @brief 字符指针，指向错误信息
    */
    const char * err_str;
    /**
    @brief 设置错误信息
    @param err 指向错误信息的指针（浅拷贝）
    */
    void SetLastError(const char * err);
public:
    /**
     * @brief 流状态标志符
     *
     * 调用status函数返回后的状态值，通过与标志符进行与运算判断是否满足条件
     * 如status()&BasicStream::FLAG::ERITEABLE>0,则代表当前流是可写的
     */
    enum FLAG{
        CLOSED=0, ///< 流关闭标志符
        READABLE=(1<<0), ///< 流可读标志符
        WRITEABLE=(1<<1), ///< 流可写标志符
        EXCEPTION=(1<<2), ///< 流异常标志符
    };
    /**
    @brief 构造函数，
     
    在构造函数中，程序会初始化缓冲区内存
    @return 无
     */
    BasicStream();
    /**
    @brief 获取缓冲区的大小
    @return 缓冲区的字节数
     */
    static size_t buffer_size();
    /**
    @brief 打开函数（备用）
    @return 是否打开成功
    @retval true 打开成功
    @retval false 打开失败,调用GetLastError获取失败原因
    */
    virtual bool open(void *);
    /**
    @brief 关闭函数
     
    关闭已经打开的流
    @return 是否关闭成功
    @retval true 关闭成功
    @retval false 关闭失败,调用GetLastError获取失败原因
    @attention close会读取内部的标志符来判断是否重复关闭，因此重复调用close是没有问题的
    */
    virtual bool close();
    /**
    @brief 读取函数
    @param[out] _dst 接收从流中读取出的字节的字符串
    @param[in] size 要读取的字节数，0代表无限制
    @return 成功从流中读取的字节数
    @retval 0 读取失败
    @retval 大于0的整数 读取成功,读取的字节会追加到传入的字符串中
     */
    virtual size_t read(std::string & _dst,size_t size = 0);
    //virtual size_t read(BasicStream & _dst,size_t size = 0);
    /**
    @brief 写入函数
    @param[in] _content 接收准备写入流的字节的字符串
    @param[in] size 要写入的字节数，0代表全部写入
    @return 成功写入流中的字节数
    @retval 0 写入失败
    @retval 大于0的整数 写入成功的字节数
     */
    virtual size_t write(const std::string & _content,size_t size = 0);
    //virtual size_t write(BasicStream & _content,size_t size = 0);
    /**
    @brief 析构函数，释放缓冲区内存
    @return 无
     */
    virtual ~BasicStream();
    /**
    @brief 状态函数,返回流的状态
    @return 状态码
     */
    int status() const;
    /**
    @brief 获取错误信息
    @return 返回最后一条错误信息
    */
    const char * GetLastError();
    size_t transport(BasicStream & _dst,size_t size = 0);
    /**
    @brief write的重载简写
    */
    friend BasicStream& operator<< (BasicStream & src, const std::string & in);
    /**
    @brief write的重载简写
    */
    friend BasicStream& operator<< (BasicStream & src, const char * in);
    /**
    @brief read的重载简写
    */
    friend BasicStream& operator>> (BasicStream & src, std::string & is);
    /**
    @brief 流与流之间的分段拷贝
    */
    friend BasicStream& operator<< (BasicStream & to, BasicStream & from);
    /**
    @brief 流与流之间的分段拷贝
    */
    friend BasicStream& operator>> (BasicStream & from, BasicStream & to);
};

typedef size_t(*BasicStringFilter)(BasicStream &,BasicStream &);

/**
 * @brief 过滤器基类
 * 
 * 所有过滤器的基类，由此可以派生出http过滤器、压缩过滤器等。
 */
class BasicFilter{
private:
    static size_t DefaultFilter(std::string &,std::string &);
protected:
    int status_id;
    const char * err_msg;
    BasicStream & src;
    BasicStream & dst;
    BasicStringFilter i_filter;
    std::string tmp;
public:
    BasicFilter(BasicStream &,BasicStream &,BasicStringFilter);
    void from(BasicStream &);
    void to(BasicStream &);
    void filter(BasicStringFilter);
    virtual ~BasicFilter();
    virtual size_t exec(size_t);
    int status(int) const;
    const char * GetLastError() const;
};

}
#endif
