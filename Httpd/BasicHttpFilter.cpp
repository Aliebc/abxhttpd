#include "include/BasicHttpFilter.hxx"
#include "include/BasicException.hxx"
#include "include/FileStream.hxx"
#include <exception>

namespace abxhttpd{
BasicHttpFilter::BasicHttpFilter(BasicStream & src, BasicStream & dst)
:BasicFilter(src, dst){
    Request=new HttpRequest();
    status_id=S_FLAG::CONTINUE_RECV;
    Response=new HttpResponse();
    send_from_stream=false;
    tmp_fstream=nullptr;
}

void BasicHttpFilter::set(HttpHandler handler,SocketRequest & req){
    Handler=handler;
    Iset=req;
}

/**
 * @brief 流过滤器函数
 * 
 * @param From 输入流
 * @param To 输出流
 * @param size 要处理的字节数量
 * @return size_t 处理成功的字节数量
 */
size_t BasicHttpFilter::StreamFilter(BasicStream & From, BasicStream & To, size_t size){
    
    tmp.clear();
    auto && _ip =Iset.src_in_ip;
    if(status_id==S_FLAG::CLOSED){
        From.close();
        return 0;
    }
    if(status_id&S_FLAG::FINISHED_WRITE){
        if(Response->header("Connection")=="keep-alive"){
            status_id|=S_FLAG::KEEP_ALIVE;
        }else{
            status_id=S_FLAG::CLOSED;
            From.close();
        }
    }
    if(status_id&S_FLAG::CONTINUE_RECV){
        From.read(tmp,size);
        Request->append(tmp);
    }else if(status_id&S_FLAG::CONTINUE_WRITE){
        if(send_from_stream){
            auto st=tmp_fstream->read(tmp,size);
            From.write(tmp);
            if(st==0){
                send_from_stream=false;
                delete tmp_fstream;
                tmp_fstream=NULL;
                status_id^=S_FLAG::CONTINUE_WRITE;
                status_id|=S_FLAG::FINISHED_WRITE;
            }
            return size;
        }
        
        if(Response->need_send_from_stream){
            tmp_fstream = new FileStream(Response->need_send_from_stream_src.c_str());
            send_from_stream=true;
            To.write(Response->raw());
            //To << *tmp_stream;
            //delete tmp_stream;
            //status_id|=S_FLAG::FINISHED_WRITE;
            //To.close();
            return 0;
        }else{
            To.write(Response->raw());
        }
        
        status_id^=S_FLAG::CONTINUE_WRITE;
        status_id|=S_FLAG::FINISHED_WRITE;
        
        return size;
    }
    try{
        Request->parse_header();
    }catch(const HttpParserException & ParserExcept){
        if(ParserExcept.code()!=BasicHttp::S_ID::NOT_FINISHED){
            HttpException BadReq(400);
            To << HttpResponse(BadReq.html(),BadReq.code()).raw() ;
            Httpd::except_logger->write(_ip +
                " [Except] Parser Error.");
            From.close();
            status_id=S_FLAG::CLOSED;
        }else{
            status_id=S_FLAG::CONTINUE_RECV;
            status_id|=S_FLAG::KEEP_ALIVE;
        }
        return size;
    }catch(const BasicException & e){
        return 0;
    }
    if(Request->length()>Request->size()){
        status_id=S_FLAG::CONTINUE_RECV;
        return 0;
    }
    status_id=S_FLAG::FINISHED_RECV;
    Request->parse_body();
    delete Response;
    Response = new HttpResponse();
    // Use Session
    status_id=S_FLAG::HANDLING;
    if(Request->cookie(ABXHTTPD_SESSION_STR).size()==0){
        auto && cook = HttpdSession::allocate();
        Response->set_cookie(ABXHTTPD_SESSION_STR, cook);
    }
    
    
    
    Request->variables("REMOTE_ADDR", _ip);
    Request->variables("REMOTE_PORT", std::to_string(Iset.port_in));
    
    if(Request->header("Connection")=="keep-alive"){
        Response->header("Connection", "keep-alive");
    }
    try{
        Handler(*Response,*Request,&Iset);
    }catch (const HttpException & e) {
        delete Response;
        Response=new HttpResponse(e.html(),e.code());
        if(e.code()==301||e.code()==302){
            Response->header("Location", e.what());
        }
        Httpd::except_logger->write(_ip +
        " [Except][" + std::to_string(e.code())+"]  "
        + e.what());
    }catch (const BasicException & e){
        Httpd::except_logger->write(_ip +
        " [Except] "
        + e.what());
        delete Response;
        HttpException exp(500);
        Response=new HttpResponse(exp.html(),exp.code());
    }catch(const std::exception & p){
        HttpException exp(500);
        Response=new HttpResponse(exp.html(),exp.code());
    }
    Httpd::success_logger->write(
    _ip + " " +
    Response->header("Content-Length")+" "+
    Request->method() + " " + Request->path() +
    " " + Request->query_string()+ " " +
    Request->header("User-Agent"));
    delete Request;
    Request=new HttpRequest();
    status_id=S_FLAG::CONTINUE_WRITE;
    tmp.clear();
    return 0;
}
BasicHttpFilter::~BasicHttpFilter(){
    delete Request;
    delete Response;
    if(send_from_stream){
        delete tmp_fstream;
    }
}
}
