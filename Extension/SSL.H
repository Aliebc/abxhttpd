#ifdef ABXHTTPD_SSL
#include "include/abxhttpd.H"
#include <openssl/ssl.h>
#include <openssl/err.h>

#define ABXHTTPD_SSL_STREAM 1

SSL_CTX* create_context()
{
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX* ctx)
{
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, global_argu->at('K').c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, global_argu->at('k').c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

namespace abxhttpd
{

    extern HttpdCoreAddress SSLHttpdCoreAddress;

    ConfigureInfo AX_SSL_INFO = { "ssl",{{"Support","enabled"},{"OpenSSL Version",OPENSSL_VERSION_TEXT}} };

    Module AX_SSL_MODULE(AX_SSL_INFO);
    class SSLSocket :public HttpdSocket {
    private:
        SSL_CTX* ctx;
        SSL* ssl;
        int ad;
    public:
        SSLSocket(SocketRequest& src) :HttpdSocket(src) {
            ctx = create_context();
            configure_context(ctx);
            ssl = SSL_new(ctx);
            ad = src._ad;
            SSL_set_fd(ssl, ad);
            if (SSL_accept(ssl) <= 0) {
                this->close();
            }
        }
        size_t read(std::string& _dst, size_t size = 0) {
            ABXHTTPD_ZERO_MEMORY(tmp);
            size_t _recv_s = 0;
            int _revc_l = 0;
            while (ssl!=NULL) {
                _revc_l = SSL_read(ssl, tmp, sizeof(tmp));
                ABXHTTPD_INFO_PRINT(11, "[Socket %d][OpenSSL API]Invoked SSL_read, returning %d.", ad, _revc_l);
                if (_revc_l > 0) {
                    _dst.insert(_dst.size(), tmp, _revc_l);
                    _recv_s += _revc_l;
                    if (_recv_s == sizeof(tmp)) {
                        continue;
                    }
                    else {
                        break;
                    }
                }
                else {
                    close();
                    break;
                }
            }
            return _recv_s;
        }
        size_t write(std::string& _res, size_t size = 0) {
            size_t _send_len = 0;
            int _send_lv = -1;
            while (true) {
                _send_lv = SSL_write(ssl, _res.c_str(), _res.size());
                ABXHTTPD_INFO_PRINT(11, "[Socket %d][OpenSSL API]Invoked SSL_write, returning %d.", ad, _send_lv);
                if (_send_lv > 0) {
                    _send_len += _send_lv;
                    if (_send_len == _res.size()) {
                        break;
                    }
                }
                else {
                    break;
                }
            }
            return _send_len;
        }
        int status() {
            return st;
        }
        bool close() {
            int close_state;
            if (st != 0) {
                SSL_shutdown(ssl);
                close_state = __close_socket(ad);
                SSL_free(ssl);
                ssl = NULL;
                SSL_CTX_free(ctx);
                ctx = NULL;
            }
            st = 0;
            ABXHTTPD_INFO_PRINT(11, "[Socket %d][OpenSSL API]Free SSL context", ad);
            return (close_state == 0);
        }
        ~SSLSocket() {
            if (st != 0) {
                close();
            }
        }
    };


    void* _ThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args, int StreamType);

    void* __SSLThreadController(const ThreadSettingList& _set, const CCore& _core, void* _args) {
        _ThreadController(_set, _core, _args, ABXHTTPD_SSL_STREAM);
        return NULL;
    }

    ThreadController ssl_ctrl = __SSLThreadController;

    HttpdCore SSLHttpdCore = {
        DefaultIFilter,
        DefaultOFilter,
        DefaultSocketI,
        DefaultSocketD,
        ssl_ctrl,
        DefaultHttpH,
    };

    HttpdCoreAddress SSLHttpdCoreAddress = {
        "SSLHttpdCore","Core using SSL, use -K(cert)/-k(key) to set SSL.",&SSLHttpdCore
    };

    HttpdCore_R SSLCore(SSLHttpdCoreAddress);

}

#endif
