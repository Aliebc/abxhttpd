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
        Request->parse();
        Handler(*Response,*Request,NULL);
    }catch(const BasicException & e){
        
    }
    return 0;
}
BasicHttpFilter::~BasicHttpFilter(){
    delete Request;
    delete Response;
}
}
