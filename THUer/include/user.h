#ifndef USER_H
#define USER_H

#include <iostream>
#include <cstring>
#include <abxhttpd/sqlite_3.H>
using namespace std;
using abxhttpd::sqlite_3_stmt;

/**
 * @brief 用户的等级
 * 
 * 总共四个等级：农民、精英、疯狂用户、管理员
 * 
 */
enum Rank{peasent = 0, elite, crazy, admin};

/**
 * @brief 用户信息结构体
 * 
 * 用来临时保存用户信息，给writeFile和readFile函数传递用户信息
 * 
 */
struct UserListStruct{
    int UserID;                     ///< 用户ID
    int UserRank = 1;               ///< 用户等级
    char UserName[32] = "";         ///< 用户名
    char Password[65] = "";         ///< 哈希后的用户密码
    long int UserDownloads = 0;     ///< 用户下载量
    long int UserUploads = 0;       ///< 用户上传量
    int UserShareRate = 0;          ///< 用户分享率
    int UserCoins = 0;              ///< 用户的硬币数量
};

/**
 * @brief 用户类
 * 
 * 用户类将与用户相关的数据和函数集成在一起
 * 用户的信息在这个类中记录
 * 用户的操作在这个类中实现
 */
class User{
private:
    Rank UserRank;              ///< 用户等级
    string UserName;            ///< 用户名
    string UserPassword;        ///< 经过哈希的用户密码
    long int UserDownloads;     ///< 用户下载量
    long int UserUploads;       ///< 用户上传量
    double UserShareRate;       ///< 用户分享率
    int UserCoins;              ///< 用户的硬币数量
    int UserID = 0;             ///< 用户ID
    static bool writeFile(const string & UserName, struct UserListStruct *CurUser);     ///< 将用户信息写入数据库的函数
    static bool readFile(const string & UserName, struct UserListStruct *CurUser);      ///< 将用户信息从数据库中读取出的函数
    static bool Verify(const string & Username, const string & Password);               ///< 比对用户名密码是否正确
    static bool CreateUser(const string & Username, const string & Password);           ///< 创建一个新用户
    double UpdateShareRate();                                                           ///< 更新用户分享率
    void SaveUser();                                                                    ///< 将用户信息保存进数据库中
public:
    User(const string & name);                                                          ///< 构造函数，以用户名创建一个对象
    User(){}                                                                            ///< 默认构造函数 
    ~User(){}                                                                           ///< 默认析构函数
    static bool RegisterNewUser(const string & Username, const string & Password);      ///< 注册新用户
    static bool Login(const string & Username, const string & Password);                ///< 用户登录
    void Logout();                                                                      ///< 用户退出
    bool ChangePassword(const string & OldPassword, const string & NewPassword);        ///< 修改密码
    double GetShareRate(){return UserShareRate;}                                        ///< 获取用户分享率
    int GetDownloads(){return UserDownloads;}                                           ///< 获取用户下载量
    int GetUploads(){return UserUploads;}                                               ///< 获取用户上传量
    string GetName(){return UserName;}                                                  ///< 获取用户名
    long int UpdateDownloads(long const DeltaD);                                         ///< 更新用户下载量
    long int UpdateUploads(long const DeltaU);                                           ///< 更新用户上传量
    int UpdateCoins(int const DeltaC);                                                  ///< 更新用户硬币数量
    string DownloadFile(int FID);                                                       ///< 下载文件
    bool UploadFile(const char * path, const char * title, const char * info, long size);///< 上传文件
};
#endif