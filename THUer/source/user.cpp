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

extern sqlite_3 dta;

/**************************
// (1) private函数的实现
***************************/

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

bool User::Verify(const string & Username, const string & Password)
{
    UserListStruct CurUser;
    if (!readFile(Username, &CurUser))
        return false;
    return (strcmp(CurUser.Password, Hash::sha256(Username+Password).c_str()) == 0);
}


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

double User::UpdateShareRate()
{
    if (UserDownloads == 0)
        UserShareRate = 0;
    else
        UserShareRate = UserUploads / UserDownloads;
    return UserShareRate;
}

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

/**************************
// (2) public函数的实现
***************************/

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

bool User::Login(const string & Username, const string & Password)
{
    return Verify(Username, Password);
}

bool User::RegisterNewUser(const string & Username, const string & Password)
{
    return CreateUser(Username, Password);
}

void User::Logout()
{
    SaveUser();
}

long int User::UpdateDownloads(int const DeltaD)
{
    if (UserDownloads + DeltaD >= 0)
        UserDownloads += DeltaD;
    else
        UserDownloads = 0;
    UpdateShareRate();
    SaveUser();
    return UserDownloads;
}

long int User::UpdateUploads(int const DeltaU)
{
    if (UserUploads + DeltaU >= 0)
        UserUploads += DeltaU;
    else
        UserUploads = 0;
    UpdateShareRate();
    SaveUser();
    return UserUploads; 
}

int User::UpdateCoins(int const DeltaC)
{
    if (UserCoins + DeltaC >= 0)
        UserCoins += DeltaC;
    else
        UserCoins = 0;
    SaveUser();
    return UserCoins;
}

bool User::ChangePassword(const string & OldPassword, const string & NewPassword)
{
    if (Hash::sha256(OldPassword) != UserPassword)
        return false;
    UserPassword = Hash::sha256(NewPassword);
    SaveUser();
    return true;
}

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
    auto ret = dta.prepare("SELECT PATH FROM F_INFO WHERE ID = ?");    
    ret->bind(1, FID);
    ret->step();
    return (const char *)(ret -> row(0));
}

bool User::UploadFile(const char * path, const char * title, const char * info)
{
    auto k = dta.prepare("INSERT INTO F_INFO (TITLE,INFO,SHARER,PATH) VALUES (?,?,?,?)");
    if (k == NULL)
    {
        throw abxhttpd::BasicException("NULL");
        return false;
    }
    k->bind(1, title);
    k->bind(2, info);
    k->bind(3, UserID);
    k->bind(4, path);
    k->step();
    return true;
}