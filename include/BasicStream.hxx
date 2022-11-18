#ifndef BASICSTREAM_H
#define BASICSTREAM_H
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include "BasicException.hxx"

namespace abxhttpd{

#define ABXHTTPD_STREAM_BUFFER 2048

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
    int status_id; ///< 状态
    char * buffer_tmp; ///<缓冲区地址
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
     @brief 构造函数
     
     在构造函数中，程序会初始化缓冲区内存
     @return 无
     */
    BasicStream();
    /// 不允许复制构造
    ///
    /// 因为对流的复制构造可能会造成串行等问题，程序应当保证流只能被一个线程控制
    BasicStream(const BasicStream&)=delete;
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
    /**
     @brief 写入函数
     @param[in] _content 接收准备写入流的字节的字符串
     @param[in] size 要写入的字节数，0代表全部写入
     @return 成功写入流中的字节数
     @retval 0 写入失败
     @retval 大于0的整数 写入成功的字节数
     */
    virtual size_t write(const std::string & _content,size_t size = 0);
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
    /// 禁止等于赋值
    void operator= (const BasicStream &) =delete;
};

/**
 * @brief 过滤器基类
 *
 * 所有过滤器的基类，由此可以派生出http过滤器、压缩过滤器等。
 */
class BasicFilter{
private:
    static size_t DefaultFilter(BasicStream &,BasicStream &);
protected:
    int status_id;
    size_t handled_length;
    const char * err_msg;
    BasicStream & source;
    BasicStream & destination;
    virtual size_t StreamFilter(BasicStream &,BasicStream &,size_t);
    std::string tmp;
public:
    /// 指定目标流的属性
    enum WHICH{
        FROM=1, ///< 源流
        TO=2 ///< 目标流
    };
    /// 指定目标流的属性
    enum B_FLAG{
        SUCCESS=0, ///< 成功执行
        EXCEPT ///< 出现异常
    };
    /// 过滤器的状态
    enum S_FLAG
    {
        CLOSED=0, ///<关闭
        CONTINUE_RECV=1<<0, ///<继续接收
        CONTINUE_WRITE=1<<1, ///<继续写入
        FINISHED_RECV=1<<2, ///< 完成接收
        FINISHED_WRITE=1<<3, ///< 完成写入
        HANDLING=1<<4, ///< 正在处理
        KEEP_ALIVE=1<<5 ///< 保持连接
    };
    /**
     @brief 构造函数
     
     接收两个流作为源流和目标流
     @param[in] src 源流
     @param[out] dst 目标流
     */
    BasicFilter(BasicStream & src,BasicStream & dst);
    virtual ~BasicFilter() = default;
    BasicFilter(const BasicFilter &) = default;
    virtual size_t exec(size_t);
    int status(int which) const;
    int status() const;
    size_t length() const;
    const char * GetLastError() const noexcept;
};

}
#endif
