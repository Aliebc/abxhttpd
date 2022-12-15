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
    void inter_del();
    bool unique;
public:
    SessionPtr();
    template <class Ty>
    inline SessionPtr(Ty * ptr):SessionPtr(){
        this->set(ptr);
    }
    template <class Ty>
    inline void set(Ty * ptr){
        inter_del();
        address=(void *)ptr;
        del=free_ptr<Ty>;
    }
    template <class Ty>
    inline Ty & cast(){
        if(null()){
            throw BasicException("Trying to cast null ptr.");
        }
        return *static_cast<Ty*>(address);
    }
    template <class Tp>
    inline void wrap(Tp * ptr){
        this->set(ptr);
    }
    bool null();
    void move(SessionPtr & s);
    void move(SessionPtr && s);
    void destory_unique();
    void destory() noexcept;
    SessionPtr (const SessionPtr &)=delete;
    SessionPtr operator=(const SessionPtr &)=delete;
    ~SessionPtr();
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
