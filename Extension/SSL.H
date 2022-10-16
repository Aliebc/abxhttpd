#ifdef ABXHTTPD_SSL
#include "include/abxhttpd.H"
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, (*global_argu)['K'].c_str(), SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, (*global_argu)['k'].c_str(), SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

namespace abxhttpd
{

extern HttpdCoreAddress SSLHttpdCoreAddress;

ConfigureInfo AX_SSL_INFO={"ssl",{{"Support","enabled"},{"OpenSSL Version",OPENSSL_VERSION_TEXT}}};

Module AX_SSL_MODULE(AX_SSL_INFO);
class SSLSocket:public HttpdSocket{
private:
    SSL_CTX *ctx;
    SSL *ssl;
    char ttmp[2048];
    int st;
public:
    int ad;
    SSLSocket(SocketRequest & src):HttpdSocket(src){
        st=1;
        ctx = create_context();
        configure_context(ctx);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, src._ad);
        ad=src._ad;
        if(SSL_accept(ssl)<=0){
            this->close();
        }
    }
    size_t read(std::string & _dst,size_t size = 0){
        ABXHTTPD_ZERO_MEMORY(ttmp);
        size_t _recv_s=0;
        int _revc_l=0;
        while(true){
            _revc_l=SSL_read(ssl,ttmp,sizeof(ttmp));
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][OpenSSL API]Invoked SSL_read, returning %d.",ad,_revc_l);
            if(_revc_l>0){
                _dst.insert(_dst.size(),ttmp,_revc_l);
                _recv_s+=_revc_l;
                if(_recv_s==sizeof(ttmp)){
                    continue;
                }else{
                    break;
                }
            }else{
                st=0;
                break;
            }
        }
        return _recv_s;
    }
    size_t write(std::string & _res,size_t size = 0){
        size_t _send_len=0;
        int _send_lv=-1;
        while(true){
            _send_lv=SSL_write(ssl,_res.c_str(),_res.size());
            ABXHTTPD_INFO_PRINT(11,"[Socket %d][OpenSSL API]Invoked SSL_write, returning %d.",ad,_send_lv);
            if(_send_lv>0){
                _send_len+=_send_lv;
                if(_send_len==_res.size()){
                    break;
                }
            }else{
                break;
            }
        }
        return _send_len;
    }
    int status(){
        return st;
    }
};

ThreadController ssl_ctrl=_ThreadController<SSLSocket>;

HttpdCore SSLHttpdCore={
    DefaultIFilter,
    DefaultOFilter,
    DefaultSocketI,
    DefaultSocketD,
    ssl_ctrl,
    DefaultHttpH,
};

HttpdCoreAddress SSLHttpdCoreAddress={
    "SSLHttpdCore","Core using SSL, use -K(cert)/-k(key) to set SSL.",&SSLHttpdCore
};

HttpdCore_R SSLCore(SSLHttpdCoreAddress);

}

#endif