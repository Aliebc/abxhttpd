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

/**
 * @brief 智能会话指针类
 *
 *
 * 内部实现了一个自动析构的智能指针, 用来在每次会话中存储信息并自动释放
 * 作用域之外会自动析构
 */
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
    /**
     * @brief 构建一个会话指针
     * 

     * @tparam Ty 类型
     * @param ptr 指针
     * @warning 使用智能会话指针后不要再手动delete
     * 示例代码
     * @code
     * SessionPtr k(new int(10));
     * @endcode
     * 
     */
    template <class Ty>
    explicit inline SessionPtr(Ty * ptr):SessionPtr(){
        this->set(ptr);
    }
    /**
     * @brief 设置内容
     * 
     * @tparam Ty 指针类型
     * @param ptr 目标指针
     */
    template <class Ty>
    inline void set(Ty * ptr){
        inter_del();
        address=(void *)ptr;
        del=free_ptr<Ty>;
    }
    /**
     * @brief 转换提取函数
     * 
     * @tparam Ty 目标类型
     * @return Ty& 返回目标的引用
     * 示例代码
     * @code
     * SessionPtr k(new int(10));
     * std::cout << k.cast<int>() << std::endl;
     *
     * @endcode
     */
    template <class Ty>
    inline Ty & cast(){
        if(null()){
            throw BasicException("Trying to cast nullptr.");
        }
        return *static_cast<Ty*>(address);
    }
    /**
     * @brief 包装函数(wrap函数的别名)
     * 
     * @tparam Tp 
     * @param ptr 
     */
    template <class Tp>
    inline void wrap(Tp * ptr){
        this->set(ptr);
    }
    /**
     * @brief 判断是否为空函数
     * 
     * @return true 内部指针为空
     * @return false 内部指针不为空
     */
    bool null();
    /**
     * @brief 转移函数
     * 
     * @param s 转移另一个会话智能指针
     */
    void move(SessionPtr & s);
    void move(SessionPtr && s);
    /**
     * @brief 摧毁唯一性

     * 为了保证只有一次析构函数, 转移会话智能指针原指针将不再能够析构
     * 
     */
    void destory_unique();
    /**
     * @brief 直接释放函数
     *
     * 直接释放内部指针
     */
    void destory();
    SessionPtr (const SessionPtr &)=delete;
    SessionPtr operator=(const SessionPtr &)=delete;
    ~SessionPtr();
};

typedef std::map <std::string,SessionPtr> SVMap;

/**
 * @brief 会话类
 * 
 */
typedef struct{
    time_t update; ///< 最后更新的时间
    SVMap pt; ///< 会话指针映射表
} SessionStruct;

/**
 * @brief 会话类
 * 
 */
class HttpdSession{
private:
    static int dead; ///< 最长存活时间
    static std::map <std::string,SessionStruct> sm_sheet; ///< 内部映射列表
    static bool check_expire(const SessionStruct &); ///< 检查是否过期
public:
    HttpdSession();
    ~HttpdSession();
    static std::string allocate();
    /**
     * @brief 设置全局过期时间
     * 
     * @param time 过期时间(单位为秒)
     */
    static void set_expire(int time);
    /**
     * @brief 全局映射表
     * 
     */
    static SVMap global;
    /**
     * @brief 判断键是否存在
     * 
     * @param _key 键
     * @return true 存在,可以读取
     * @return false 不存在,不可以读取
     */
    static bool can_get(const std::string & _key);
    /**
     * @brief 获取键对应的值
     * 
     * @param _key 键
     * @return SVMap& 获取会话映射表
     */
    static SVMap & get(const std::string & _key);
    /**
     * @brief 删除键对应的会话映射表
     * 
     * @param _key 键
     * @attention 调用后会话指针将释放会话映射表中所有的内存
     */
    static void del(const std::string & _key);
};

inline void SessionPtr::move(SessionPtr && s){
    inter_del();
    s.destory_unique();
    address=s.address;
    del=s.del;
    s.address=NULL;
    s.del=NULL;
}

inline void SessionPtr::move(SessionPtr & s){
    this->move(std::move(s));
}

inline bool SessionPtr::null(){
    return (address==NULL);
}

inline SessionPtr::SessionPtr(){
    address=NULL;
    del=NULL;
    unique=true;
}

inline void SessionPtr::destory(){
        inter_del();
}

inline void SessionPtr::destory_unique(){
    unique=false;
}

inline void SessionPtr::inter_del(){
    if((address!=NULL)&&(unique==true)){
        del(address);
        del=NULL;
        address=NULL;
    }
}

inline SessionPtr::~SessionPtr(){inter_del();}


}

#endif
