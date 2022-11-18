#include "include/BasicHttpFilter.hxx"

namespace abxhttpd{
BasicHttpFilter::BasicHttpFilter(BasicStream & src, BasicStream & dst)
:BasicFilter(src, dst){
    Request=new HttpRequest();
    Response=new HttpResponse();
}

void BasicHttpFilter::set_handler(HttpHandler handler){
    this->Handler=handler;
}

size_t BasicHttpFilter::StreamFilter(BasicStream & f, BasicStream & t, size_t size){
    tmp.clear();
    f.read(tmp,size);
    Request->append(tmp);
    try{
        Request->parse_header();
    }catch(const BasicException & e){
        
    }catch(const HttpParserException & ParserError){
        if(ParserError.code()!=BasicHttp::S_ID::NOT_FINISHED){
            this->status_id=S_FLAG::CLOSED;
        }else{
            this->status_id=S_FLAG::CONTINUE_RECV;
        }
    }
    Request->parse_body();
    Handler(*Response,*Request,NULL);
    tmp.clear();
    return 0;
}
BasicHttpFilter::~BasicHttpFilter(){
    delete Request;
    delete Response;
}
}
