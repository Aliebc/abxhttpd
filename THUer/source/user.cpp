#include "../include/user.h"
#include <iostream>
using namespace std;

/**************************
// (1) private函数的实现
***************************/


int User::ReportError(string ErrorType)
{
    cout << endl << "ERROR: " << ErrorType << endl;
    return 0;
}

int User::ReportInfo(string Info)
{
    cout << endl << Info << endl;
    return 0;
}

int User::FindWritable()
{
    int i = 0;
    while (true)
    {
        if (i >= MAXN)
            break;              ///< 超出用户数上限就停下
        if (!UserList[i].ValidUser)
            break;              ///< 找到第一个不合法用户的位置
        i++;
    }
    if (i == MAXN)
        ReportError("UserNum exceeded");
    return i;
}

int User::FindUser(const string & Username)
{
    ///< 【暂用】复杂度为O(n)的查找
    int i = -1;
    while (true)
    {
        i++;
        if (i >= MAXN)
            break;              ///< 超出用户数上限就停下
        if (!UserList[i].ValidUser)
            continue;           ///< 跳过不合法用户
        if (UserList[i].UserName == Username)
            break;              ///< 找到对应用户就停下       
    }
    return i;
}


bool User::Verify(const string & Username, const string & Password)
{
    int pos = FindUser(Username);
    if (pos == MAXN)
    {
        ReportInfo("No such user");
        return false;
    }
    return UserList[pos].Password == Password;
}

bool User::Verify(const string & Password)
{
    return UserList[UserPos].Password == Password;
}

void User::ReadUserInfo()
{
    UserRank = UserList[UserPos].UserRank;
    UserName = UserList[UserPos].UserName;
    UserDownloads = UserList[UserPos].UserDownloads;
    UserUploads = UserList[UserPos].UserUploads;
    UserShareRate = UserList[UserPos].UserShareRate;
    UserCoins = UserList[UserPos].UserCoins;
}

bool User::CreateUser(const string & Username, const string & Password)
{
    int Pos = FindWritable();
    if (Pos == MAXN)
        return false;
    UserList[Pos].ValidUser = true;          ///< 修改为合法账号
    UserList[Pos].UserRank = peasent;        ///< 初始等级
    UserList[Pos].UserName = Username;
    UserList[Pos].Password = Password;
    UserList[Pos].UserDownloads = 0;          ///< 初始用户下载量
    UserList[Pos].UserUploads = 0;            ///< 初始用户上传量
    UserList[Pos].UserShareRate = 0;          ///< 初始用户分享率
    UserList[Pos].UserCoins = 0;              ///< 初始用户硬币数量
    return true;
}

double User::UpdateShareRate()
{
    if (UserDownloads == 0)
        UserShareRate = 0;
    else
        UserShareRate = UserUploads / UserDownloads;
    return UserShareRate;
}

void User::ResetPassWord(const string &NewPassword)
{
    UserList[UserPos].Password = NewPassword;
}

/**************************
// (2) public函数的实现
***************************/

bool User::CheckIfAlreadyExist(const string & Username)
{
    int pos = FindUser(Username);
    return !(pos == MAXN);  
}

bool User::Login(const string & Username, string & Password)//这里要返回位置而不是成功
{
    bool success = Verify(Username, Password);
    if (success)
        ReportInfo("登录成功！");
    else
    {
        ReportInfo("密码或用户名错误");
        string ErrorReport = "wrong username or wrong password";
        throw ErrorReport;
    } 
        
    return success;
}

bool User::RegisterNewUser(const string & Username, string & Password)
{
    bool success = CreateUser(Username, Password);
    if (success)
        ReportInfo("注册成功！");
    else 
        ReportInfo("注册失败！");
    return success;
}

User::User(const string & name)
{
    UserPos = FindUser(name);
    ReadUserInfo();
    cout << "成功创建了一个用户" << endl;
}///<【暂用】

bool User::ChangeUserPassword(const string & NewPassword, string & Password)
{
    if (!Verify(Password))
    {
        ReportInfo("原密码错误!");
        return false;
    }
    ResetPassWord(NewPassword);
    return true;
}


int main()
{
    cout << "按下对应的数字键选择操作\n1：登录\t2：注册" << endl;
    int command;
    cin >> command;
    string name, password;
    if (command == 1)
    {
        cout << "请输入用户名" << endl;
        cin >> name;
        cout << "请输入密码" << endl;
        cin >> password;
        try
        {
            User::Login(name, password);
        } 
        catch (string &e)
        {
            cerr << "Error: " << e << endl;
        }
         
    }
    else if (command == 2)
    {
        string password1, password2;
        cout << "请输入用户名" << endl;
        cin >> name;
        while (User::CheckIfAlreadyExist(name))
        {
            cout << "用户名已存在，请换一个" << endl;
            cin >> name;
        }
        while (true)
        {
            cout << "请输入密码" << endl;
            cin >> password1;
            cout << "请再次输入密码" << endl;
            cin >> password2;
            if (password1 == password2)
                break;
            cout << "两次输入不一致，请重新输入" << endl;
        }   
        User::RegisterNewUser(name, password1);
    }
    else
    {
        cout << "错误输入" << endl;
    }
    return 0; 
}