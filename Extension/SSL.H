#ifdef ABXHTTPD_SSL
#include "include/abxhttpd.H"
#include <openssl/ssl.h>
#include <openssl/err.h>

#define ABXHTTPD_SSL_STREAM 1

SSL_CTX* create_context();

void configure_context(SSL_CTX* ctx);

namespace abxhttpd
{
    extern HttpdCoreAddress SSLHttpdCoreAddress;
    class SSLSocket :public HttpdSocket {
    private:
        SSL_CTX* ctx;
        SSL* ssl;
        int ad;
    public:
        SSLSocket(SocketRequest& src);
        size_t read(std::string& _dst, size_t size = 0) override;
        size_t write(const std::string& _res, size_t size = 0) override;
        bool close() override;
        ~SSLSocket();
    };


    void* _ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args, int StreamType);

    void* __SSLThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args);
}

#endif
