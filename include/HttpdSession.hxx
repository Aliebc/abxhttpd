#ifndef HTTPD_SESSION
#define HTTPD_SESSION

#include <string>
#include <map>
#include <random>
#include <ctime>

#include "BasicException.hxx"

#define ABXHTTPD_SESSION_STR "ABXHTTPD-SESSIONID"
#define ABXHTTPD_SESSION_IDLEN 32
#define ABXHTTPD_SESSION_DEAD 600
#define ABXHTTPD_SESSION_UNIQUE false

namespace abxhttpd{

class SessionPtr{
private:
    template <class P>
    static void free_ptr(void * ptr){
        delete static_cast<P*>(ptr);
    }
    void * address;
    void (*del)(void *);
    void inter_del(){
        if((address!=NULL)&&(unique==true)){
            del(address);
            del=NULL;
            address=NULL;
        }
    }
    bool unique;
public:
    SessionPtr(){
        address=NULL;
        del=NULL;
        unique=true;
    }
    template <class Ty>
    SessionPtr(Ty * ptr):SessionPtr(){
        this->set(ptr);
    }
    template <class Ty>
    void set(Ty * ptr){
        inter_del();
        address=(void *)ptr;
        del=free_ptr<Ty>;
    }
    void move(SessionPtr & s){
        inter_del();
        s.destory_unique();
        address=s.address;
        del=s.del;
        s.address=NULL;
        s.del=NULL;
    }
    bool null(){
        return (address==NULL);
    }
    void move(SessionPtr && s){
        inter_del();
        s.destory_unique();
        address=s.address;
        del=s.del;
        s.address=NULL;
        s.del=NULL;
    }
    template <class Tp>
    void wrap(Tp * ptr){
        this->set(ptr);
    }
    template <class Ty>
    Ty & cast(){
        if(null()){
            throw BasicException("Trying to cast null ptr.");
        }
        return *static_cast<Ty*>(address);
    }
    void destory_unique(){
        unique=false;
    }
    void destory(){
        inter_del();
    }
    SessionPtr (const SessionPtr &)=delete;
    SessionPtr operator=(const SessionPtr &)=delete;
    ~SessionPtr(){inter_del();}
};



typedef std::map <std::string,SessionPtr> SVMap;

typedef struct{
    time_t update;
    SVMap pt;
} SessionStruct;

class HttpdSession{
private:
    static int dead;
    static int all_id;
    static std::map <std::string,SessionStruct> sm_sheet;
    static bool check_expire(const SessionStruct &);
public:
    HttpdSession();
    ~HttpdSession();
    static std::string allocate();
    static void set_expire(int);
    static SVMap global;
    static bool can_get(const std::string & _key);
    static SVMap & get(const std::string & _key);
    static void del(const std::string & _key);
};
}

#endif
