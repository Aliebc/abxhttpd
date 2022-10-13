#include "HttpRequest.hxx"

namespace abxhttpd{
    class JsonRequest: public HttpRequest
    {
    private:
        /* data */
    public:
        JsonRequest(/* args */);
        ~JsonRequest();
    };
}
