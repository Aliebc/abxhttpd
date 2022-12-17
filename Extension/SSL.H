#ifdef ABXHTTPD_SSL
#include "include/HttpdSocket.hxx"
#include "include/config.hxx"
#include <openssl/ssl.h>
#include <openssl/err.h>

#define ABXHTTPD_SSL_STREAM 1

namespace abxhttpd
{
    extern HttpdCoreAddress SSLHttpdCoreAddress;
    /**
     *@brief SSL(安全套接字协议)拓展模块
     *
     *SSL流，用于安全的流传输，其可以通过TLS的方式建立安全通道，保证信息不被窃取或篡改
     */
    class SSLSocket :public HttpdSocket {
    private:
        SSL* ssl; ///< SSL通道
        int ad; ///< 套接字描述符
        static void configure_context(SSL_CTX* ctx);
        static SSL_CTX* create_context();
        static SSL_CTX * global_ctx; ///< SSL上下文
    public:
        SSLSocket(const SocketRequest& src);///< 构造函数，同基类的构造方式一样
        size_t read(std::string& _dst, size_t size = 0) override;
        size_t write(const std::string& _res, size_t size = 0) override;
        bool close() override;
        static void * init_ssl(void *);
        ~SSLSocket();
    };


    void* _ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args, int StreamType);

    void* __SSLThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args);
}

#endif
