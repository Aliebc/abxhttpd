#include "../include/user.h"
#include "../include/database.h"
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

/**************************
// (1) private函数的实现
***************************/


string hash_a(const string & _src){
    char dst[65], *_dst = dst;
    SHA256_CTX tmp;
    unsigned char tmpc[32]={0};
    SHA256_Init(&tmp);
    SHA256_Update(&tmp,_src.c_str(),_src.size());
    SHA256_Final(tmpc,&tmp);
    for(int j=0;j<sizeof(tmpc);++j){
        snprintf(_dst,3,"%02x",tmpc[j]);
        _dst+=2;
    }
    return (string)dst;
}

FILE* OpenFile(const string & fileName, const char * method)
{
    char path[64];
    snprintf(path, sizeof(path), "./data/user/%s", hash_a(fileName).c_str());
    return fopen(path, method);
}

bool User::readFile(const string & UserName, struct UserListStruct *CurUser)
{
    sqlite_3 dta("./thuer.db");
    std::unique_ptr<sqlite_3_stmt> dta_usr(dta.prepare(
        "SELECT USERNAME,PASSWORD,DOWNLOAD,UPLOAD,COIN,RANK FROM U_INFO WHERE USERNAME = ?"
        ));
    dta_usr->bind(1,UserName.c_str());
    if(dta_usr->step()==SQLITE_ROW){
        strcpy(CurUser->UserName,(const char *)dta_usr->row(0));
        strcpy(CurUser->Password,(const char *)dta_usr->row(1));
        dta_usr->row(2,&CurUser->UserDownloads);
        dta_usr->row(3,&CurUser->UserUploads);
        dta_usr->row(4,&CurUser->UserCoins);
        dta_usr->row(5,&CurUser->UserRank);
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
    sqlite_3 dta("./thuer.db");
    std::unique_ptr<sqlite_3_stmt> dta_usr(dta.prepare(
        "INSERT INTO U_INFO (USERNAME,PASSWORD) VALUES (?,?) ON CONFLICT(USERNAME) DO \
        UPDATE SET PASSWORD=?"
    ));
    dta_usr->bind(1,CurUser->UserName);
    dta_usr->bind(2,CurUser->Password);
    dta_usr->bind(3,CurUser->Password);
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
    return (strcmp(CurUser.Password, hash_a(Password).c_str()) == 0);
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
        snprintf(CurUser.Password, sizeof(CurUser.Password), "%s", hash_a(Password).c_str());
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
    return UserDownloads;
}

long int User::UpdateUploads(int const DeltaU)
{
    if (UserUploads + DeltaU >= 0)
        UserUploads += DeltaU;
    else
        UserUploads = 0;
    UpdateShareRate();
    return UserUploads; 
}

int User::UpdateCoins(int const DeltaC)
{
    if (UserCoins + DeltaC >= 0)
        UserCoins += DeltaC;
    else
        UserCoins = 0;
    return UserCoins;
}

bool User::ChangePassword(const string & OldPassword, const string & NewPassword)
{
    if (hash_a(OldPassword) != UserPassword)
        return false;
    UserPassword = hash_a(NewPassword);
    SaveUser();
    return true;
}

int main()
{
    /*if (User::Login("TTDiang", "123456"))
       cout << "登录成功" << endl;
    else
       cout << "登录失败" << endl;
    User::RegisterNewUser("TTDiang", "123456");
    if (User::Login("TTDiang", "12345"))
       cout << "登录成功" << endl;
    else
       cout << "登录失败" << endl;
    string name = "TTDiang";
    User TTDiang(name);
    TTDiang.ChangePassword("123456", "12345");
    if (User::Login("TTDiang", "12345"))
       cout << "登录成功" << endl;
    else
       cout << "登录失败" << endl;
    */
    if (User::Login("xiang", "xiang")){
        User x("xiang");
        cout << "登录成功" << endl;
    }else{
        cout << "登录失败" << endl;
    }
    //User::RegisterNewUser("TTDiang", "123456");
    
    cout << ((User::RegisterNewUser("TTDiang2", "1234567"))?"True":"False")<<endl;
    return 0; 
}