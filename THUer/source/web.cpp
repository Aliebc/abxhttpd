#include "user.h"
#include <abxhttpd/abxhttpd.H>
#include "libcaptcha.h"

void Handl(abxhttpd::HttpResponse & Response,const abxhttpd::HttpRequest & Request, void * args){
    if(Request.path()=="/"){
        if(!Request.session("u_info")->null()){
            Response.location("/main.html");
        }
    }
    if(Request.path()=="/login"){
        if(Request.sessionA<string>("u_info_captcha")!=Request.request("captcha")){
            Response.location("/logerr_captcha.html");
        }else{
            if(User::Login(Request.request("username"),Request.request("password"))){
                Request.session("u_info")->wrap(new User(Request.request("username")));
                Response.location("/main.html");
            }else{
                Response.location("/logerr_password.html");
            }
        }
    }else if(Request.path()=="/reg"){
        if(Request.sessionA<string>("u_info_captcha")!=Request.request("captcha")){
            Response.location("/regerr_captcha.html");
        }else{
            if((Request.request("password").size()&Request.request("username").size())==0){
                Response.location("/regerr.html");
            }else{
                if(User::RegisterNewUser(Request.request("username"),Request.request("password"))){
                    Response.location("/reg_success.html");
                }else{
                    Response.location("/regerr.html");
                }
            }
        }
    }else if(Request.path()=="/info"){
        char tmp[2048]={0};
        if(Request.session("u_info")->null()){
            snprintf(tmp,sizeof(tmp),"{\"status\":false}");
        }else{
            Response.header("Content-Type","application/json");
            snprintf(tmp,sizeof(tmp),
            "{\"status\":true,\"username\":\"%s\"}",
            Request.sessionA<User>("u_info").GetName().c_str()
            );
        }
        Response.write(tmp);
    }else if(Request.path()=="/quit"){
        Request.session("u_info")->destory();
        Response.location("/");
    }else if(Request.path()=="/count"){
        if(Request.session("count")->null()){
            Request.session("count")->wrap(new int(0));
        }
        if(Request.global("count")->null()){
            Request.global("count")->wrap(new int(0));
        }
        ++Request.sessionA<int>("count");
        ++Request.globalA<int>("count");
        Response.write("Times you explore this page: ");
        Response.writeA(Request.sessionA<int>("count"));
        Response.write("<br>Times all people explore this page: ");
        Response.writeA(Request.globalA<int>("count"));
    }else if(Request.path()=="/thuer-captcha"){
        char letter[6];
	    unsigned char im[70*200];
        char gif[17646];
        captcha((unsigned char *)&im,(unsigned char *)&letter);
        makegif(im,(unsigned char *)&gif);
        Request.session("u_info_captcha")->wrap(new string(letter));
        string kgif(gif,sizeof(gif));
        Response.header("Content-type","image/gif");
        Response.write(kgif);
    }else{
        abxhttpd::DefaultHttpH(Response,Request,args);
    }
}

int main()
{
    abxhttpd::HttpdSettingList setl{};
    setl.Http_S.Path="static";
    setl.Socket_S.Port=8889;
    abxhttpd::HttpdCore xcore=abxhttpd::DefaultHttpdCore;
    xcore.Handler=Handl;
    try{
        abxhttpd::Httpd testc(xcore,setl);
        testc.start();
    }catch (const abxhttpd::BasicException & e){
        cout << e.what() <<endl;
        getchar();
    }
    return 0; 
}