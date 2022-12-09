#ifndef USER_H
#define USER_H

#include <iostream>
#include <cstring>
#include <abxhttpd/sqlite_3.H>
using namespace std;
using abxhttpd::sqlite_3_stmt;

enum Rank{peasent = 0, elite, crazy, admin};
struct UserListStruct{
    ///< 用来保存用户信息
    int UserID;
    int UserRank = 1; 
    char UserName[32] = "";
    char Password[65] = "";
    long int UserDownloads = 0;          ///< 用户下载量
    long int UserUploads = 0;            ///< 用户上传量
    int UserShareRate = 0;          ///< 用户分享率
    int UserCoins = 0;              ///< 用户的硬币数量
};


class User{
private:
    ///< 数据成员
    Rank UserRank;              ///< 用户等级，分成四个等级：农民、精英、疯狂用户、管理员
    string UserName;            ///< 用户名
    string UserPassword;
    long int UserDownloads;          ///< 用户下载量
    long int UserUploads;            ///< 用户上传量
    double UserShareRate;       ///< 用户分享率
    int UserCoins;              ///< 用户的硬币数量
    int UserID = 0;
    ///< 函数成员
    static bool writeFile(const string & UserName, struct UserListStruct *CurUser);
    static bool readFile(const string & UserName, struct UserListStruct *CurUser);
    static int FindUser(const string & Username);                                ///< 找用户在UserList中的位置，没有找到的话返回的就是应该插入的位置
    static bool Verify(const string & Username, const string & Password);        ///< 检查用户名密码是否正确
    static bool CreateUser(const string & Username, const string & Password);      ///< 写入用户信息
    double UpdateShareRate();                                                       ///< 更新分享率
    void SaveUser();
    sqlite_3_stmt * FileDownloaded(); 
public:
    User(const string & name);
    User(){}
    ~User(){}                                                                    ///< 建立最初始用户列表
    static bool RegisterNewUser(const string & Username, const string & Password);     ///< 创建新用户
    static bool Login(const string & Username, const string & Password);               ///< 登录
    void Logout();
    bool ChangePassword(const string & OldPassword, const string & NewPassword);
    double GetShareRate(){return UserShareRate;}
    int GetDownloads(){return UserDownloads;}
    int GetUploads(){return UserUploads;}
    string GetName(){return UserName;}
    long int UpdateDownloads(int const DeltaD);
    long int UpdateUploads(int const DeltaU);
    int UpdateCoins(int const DeltaC);
    string DownloadFile(int FID);
    bool UploadFile(const char * path, const char * title, const char * info);
};
#endif