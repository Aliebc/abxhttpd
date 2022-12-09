#include "../include/user.h"
#include <abxhttpd/sqlite_3.H>
#include <abxhttpd/hash.H>
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

using abxhttpd::sqlite_3;
using abxhttpd::sqlite_3_stmt;
using abxhttpd::Hash;

extern sqlite_3 dta;    ///< 数据库，保存了用户信息和文件信息

/**
 * @brief 读取数据库
 * 
 * 从数据库中读取某个特定用户名的用户的信息，并把读取到的内容保存在一个传入的结构体里
 * @param UserName 用户名
 * @param CurUser 保存用户信息的临时结构体
 * @return true 读取成功
 * @return false 读取失败，用户未找到
 */
bool User::readFile(const string & UserName, struct UserListStruct *CurUser)
{
    std::unique_ptr<sqlite_3_stmt> dta_usr(dta.prepare(
        "SELECT ID,USERNAME,PASSWORD,DOWNLOAD,UPLOAD,COIN,RANK FROM U_INFO WHERE USERNAME = ?"
        ));
    dta_usr->bind(1,UserName.c_str());
    if(dta_usr->step()==SQLITE_ROW){
        strcpy(CurUser->UserName,(const char *)dta_usr->row(1));
        strcpy(CurUser->Password,(const char *)dta_usr->row(2));
        dta_usr->row(3,&CurUser->UserDownloads);
        dta_usr->row(4,&CurUser->UserUploads);
        dta_usr->row(5,&CurUser->UserCoins);
        dta_usr->row(6,&CurUser->UserRank);
        dta_usr->row(0,&CurUser->UserID);
        if(CurUser->UserDownloads!=0){
            CurUser->UserShareRate=CurUser->UserUploads/CurUser->UserDownloads;
        }
        return true;
    }else{
        return false;
    }
}


/**
 * @brief 写入数据库
 * 
 * 将用户信息写入数据库之中，用户信息用一个结构体传入
 * @param UserName 用户名
 * @param CurUser 保存用户信息的临时结构体
 * @return true 写入成功
 * @return false 写入失败
 */
bool User::writeFile(const string & UserName, struct UserListStruct *CurUser)
{
    std::unique_ptr<sqlite_3_stmt> dta_usr(dta.prepare(
        "INSERT INTO U_INFO (USERNAME,PASSWORD,DOWNLOAD,UPLOAD,COIN,RANK) VALUES (?,?,?,?,?,?)"
    ));
    dta_usr->bind(1,CurUser->UserName);
    dta_usr->bind(2,CurUser->Password);
    dta_usr->bind(3,CurUser->UserDownloads);
    dta_usr->bind(4,CurUser->UserUploads);
    dta_usr->bind(5,CurUser->UserCoins);
    dta_usr->bind(6,CurUser->UserRank);
    if(dta_usr->step()==SQLITE_DONE){
        return true;
    }else{
        return false;
    }
}


/**
 * @brief 检验用户密码是否正确
 * 
 * 读入用户名和密码，检验该用户的密码是否正确
 * @param Username 用户名
 * @param Password 用户密码
 * @return true 密码正确
 * @return false 密码错误
 */
bool User::Verify(const string & Username, const string & Password)
{
    UserListStruct CurUser;
    if (!readFile(Username, &CurUser))
        return false;
    return (strcmp(CurUser.Password, Hash::sha256(Username+Password).c_str()) == 0);
}


/**
 * @brief 创建新用户
 * 
 * 以一个用户名和一个密码创建新用户。如果用户名重复，则不创建
 * @param Username 用户名
 * @param Password 密码
 * @return true 创建成功
 * @return false 创建失败
 */
bool User::CreateUser(const string & Username, const string & Password)
{
    UserListStruct CurUser;
    bool if_exist = readFile(Username, &CurUser);
    if (if_exist)
        return false;
    else
    {
        CurUser.UserCoins = 0;
        snprintf(CurUser.Password, sizeof(CurUser.Password), "%s", Hash::sha256(Username+Password).c_str());
        CurUser.UserDownloads = 0;
        CurUser.UserUploads = 0;
        snprintf(CurUser.UserName, sizeof(CurUser.UserName), "%s", Username.c_str());
        CurUser.UserRank = 0;
        CurUser.UserShareRate = 0;
        writeFile(Username, &CurUser);
        return true;
    }
}


/**
 * @brief 更新分享率
 * 
 * 更新用户的分享率。规则为，如果下载量为0，则分享率为0；如果下载量非0，则分享率 = 上传量 / 下载量
 * @return double 分享率
 */
double User::UpdateShareRate()
{
    if (UserDownloads == 0)
        UserShareRate = 0;
    else
        UserShareRate = UserUploads / UserDownloads;
    return UserShareRate;
}


/**
 * @brief 保存用户信息
 * 
 * 将用户信息从当前对象中读取出，然后保存入数据库里
 * 
 */
void User::SaveUser()
{
    UserListStruct CurUser;
    CurUser.UserCoins = UserCoins;
    snprintf(CurUser.Password, sizeof(CurUser.Password), "%s", UserPassword.c_str());
    CurUser.UserDownloads = UserDownloads;
    CurUser.UserUploads = UserUploads;
    snprintf(CurUser.UserName, sizeof(CurUser.UserName), "%s", UserName.c_str());
    CurUser.UserRank = UserRank;
    CurUser.UserShareRate = UserShareRate;
    writeFile(UserName, &CurUser);
}


/**
 * @brief 创建User对象
 * 
 * @param name 用户名
 */
User::User(const string & name)
{
    UserListStruct CurUser;
    readFile(name, &CurUser);
    UserRank = (Rank) CurUser.UserRank;
    UserCoins = CurUser.UserCoins;
    UserDownloads = CurUser.UserDownloads;
    UserUploads = CurUser.UserUploads;
    UserShareRate = CurUser.UserShareRate;
    UserPassword = CurUser.Password;
    UserName = (string)CurUser.UserName;
}


/**
 * @brief 登录
 * 
 * 以用户名和密码登录
 * 
 * @param Username 用户名
 * @param Password 密码
 * @return true 登录成功
 * @return false 登录失败
 */
bool User::Login(const string & Username, const string & Password)
{
    return Verify(Username, Password);
}


/**
 * @brief 注册用户
 * 
 * 以用户名和密码注册用户
 * @param Username 用户名
 * @param Password 密码
 * @return true 注册成功
 * @return false 注册失败
 */
bool User::RegisterNewUser(const string & Username, const string & Password)
{
    return CreateUser(Username, Password);
}


/**
 * @brief 登出
 * 
 * 登出，并保存用户信息
 */
void User::Logout()
{
    SaveUser();
}


/**
 * @brief 更新用户下载量
 * 
 * 更新用户的下载量，同时自动更新分享率
 * @param DeltaD 下载量的变化值
 * @return long int 更新后的下载量
 */
long int User::UpdateDownloads(long const DeltaD)
{
    if (UserDownloads + DeltaD >= 0)
        UserDownloads += DeltaD;
    else
        UserDownloads = 0;
    UpdateShareRate();
    SaveUser();
    return UserDownloads;
}


/**
 * @brief 更新用户上传量
 * 
 * 更新用户的上传量，同时自动更新分享率
 * @param DeltaU 上传量的变化值
 * @return long int 更新后的上传量
 */
long int User::UpdateUploads(long const DeltaU)
{
    if (UserUploads + DeltaU >= 0)
        UserUploads += DeltaU;
    else
        UserUploads = 0;
    UpdateShareRate();
    SaveUser();
    return UserUploads; 
}


/**
 * @brief 更新用户硬币数量
 * 
 * 更新用户的硬币数量
 * @param DeltaC 硬币数量的变化值
 * @return int 更新后的硬币数量
 */
int User::UpdateCoins(int const DeltaC)
{
    if (UserCoins + DeltaC >= 0)
        UserCoins += DeltaC;
    else
        UserCoins = 0;
    SaveUser();
    return UserCoins;
}


/**
 * @brief 更改密码
 * 
 * 更改用户密码，要求输入旧密码，如果旧密码错误就直接返回false，不更改密码 
 * @param OldPassword 旧密码
 * @param NewPassword 新密码
 * @return true 更改成功
 * @return false 更改失败
 */
bool User::ChangePassword(const string & OldPassword, const string & NewPassword)
{
    if (Hash::sha256(OldPassword) != UserPassword)
        return false;
    UserPassword = Hash::sha256(NewPassword);
    SaveUser();
    return true;
}


/**
 * @brief 下载文件
 * 
 * 下载文件，传入文件的ID，返回文件的地址 
 * @param FID 文件ID
 * @return string 文件地址
 */
string User::DownloadFile(int FID)
{
    auto k = dta.prepare("UPDATE F_INFO SET Download = Download+1 WHERE ID = ?");  
    if (k == NULL)
    {
        throw abxhttpd::BasicException("NULL");
        return " ";
    }
    k->bind(1, FID);
    k->step();
    auto ret = dta.prepare("SELECT PATH,SIZE FROM F_INFO WHERE ID = ?");    
    ret->bind(1, FID);
    ret->step();
    UpdateDownloads(atol((const char *)(ret -> row(1))));
    return (const char *)(ret -> row(0));
}


/**
 * @brief 上传文件
 * 
 * 上传文件，传入文件地址、文件标题和文件简介，然后
 * @param path 
 * @param title 
 * @param info 
 * @return true 
 * @return false 
 */
bool User::UploadFile(const char * path, const char * title, const char * info, long size)
{
    auto k = dta.prepare("INSERT INTO F_INFO (TITLE,INFO,SHARER,PATH,SIZE) VALUES (?,?,?,?,?)");
    if (k == NULL)
    {
        throw abxhttpd::BasicException("NULL");
        return false;
    }
    k->bind(1, title);
    k->bind(2, info);
    k->bind(3, UserID);
    k->bind(4, path);
    k->bind(5, size);
    k->step();
    UpdateUploads(size);
    return true;
}