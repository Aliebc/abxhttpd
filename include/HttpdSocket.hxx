#ifndef HTTPDSOCKET_H
#define HTTPDSOCKET_H

#include "MacroFunctions.hxx"
#include "Httpd.hxx"
#include "BasicStream.hxx"
#ifdef ABXHTTPD_WINDOWS

#define MSG_DONTWAIT 0x0

#endif

#define ABXHTTPD_SOCK_STREAM 0

namespace abxhttpd{
    /**
     * @brief 套接字流
     *
     * 封装了对套接字的基本操作，内部实现是维护了一个套接字描述符，依据不同操作系统封装读写方法
     *
     * 套接字流可以被继承，可以被认为是一种可插拔过滤器
     *
     *
     */
    class ABXHTTPD_API HttpdSocket:public BasicStream
    {
    public:
        /// 默认构造函数
        HttpdSocket();
        /**
         * @brief 构造函数（通过SocketRequest）
         * @param Request 目标请求
         * @attention 此函数会深拷贝SocketRequest，可以放心释放请求元素
         *
         */
        explicit HttpdSocket(const SocketRequest & Request);
        /// 查看内部的请求信息
        const SocketRequest & info() const;
        virtual bool close() override;
        virtual size_t read(std::string & _dst,size_t size = 0 ) override;
        virtual size_t write(const std::string & _content,size_t size = 0) override;
        /// 析构函数
        virtual ~HttpdSocket();
        /// 跨平台的关闭套接字函数
        /// @param ad 套接字描述符
        /// @retval 0 成功关闭
        /// @retval 其他 无法成功关闭
        static int __close_socket(int ad);
    protected:
        /// 内部套接字请求信息
        SocketRequest _src;
    };
}

#endif
