#include <iostream>
#include <cstring>
#define MAXN 100  ///< 暂时设置的最大用户数量
using namespace std;
enum Rank{peasent = 1, elite, crazy, admin};
struct UserListStruct{
    ///< 暂时用来保存用户信息
    bool ValidUser = false;     ///< 默认是不能使用的账号
    Rank UserRank; 
    string UserName;
    string Password;
    int UserDownloads;          ///< 用户下载量
    int UserUploads;            ///< 用户上传量
    int UserShareRate;          ///< 用户分享率
    int UserCoins;              ///< 用户的硬币数量
}UserList[MAXN];



class User{
private:
    ///< 数据成员
    //enum Rank{peasent = 1, elite, crazy, admin};
    Rank UserRank;              ///< 用户等级，分成四个等级：农民、精英、疯狂用户、管理员
    string UserName;            ///< 用户名
    int UserDownloads;          ///< 用户下载量
    int UserUploads;            ///< 用户上传量
    double UserShareRate;       ///< 用户分享率
    int UserCoins;              ///< 用户的硬币数量
    int UserPos;                    ///< 【暂用】这个变量用来存当前用户在UserList的位置
    ///< 函数成员
    static int FindUser(const string & Username);                                ///< 找用户在UserList中的位置，没有找到的话返回的就是应该插入的位置
    static int FindWritable();                                                   ///< 【暂用】返回UserList数组中可以写入的位置
    static bool Verify(const string & Username, const string & Password);        ///< 检查用户名密码是否正确
    static bool CreateUser(const string & Username, const string & Password);      ///< 写入用户信息
    static int ReportError(string ErrorType);                                    ///< 汇报程序的错误
    static int ReportInfo(string Info);                                          ///< 用户操作错误时给用户提供信息
    void ReadUserInfo();
    void ResetPassWord(const string &NewPassword);
    bool Verify(const string & Password);                       ///
    double UpdateShareRate();                                                       ///< 更新分享率
public:
    User(const string & name);
    User(){}
    ~User(){}
    //static bool Init();                                                          ///< 建立最初始用户列表
    static bool CheckIfAlreadyExist(const string & Username);                    ///< 检查是否已经存在这个用户名
    static bool RegisterNewUser(const string & Username, string & Password);     ///< 创建新用户
    static bool Login(const string & Username, string & Password);               ///< 登录
    bool Logout();
    bool ChangeUserName(const string & NewUsername, string & Password);        ///< 更改用户名
    bool ChangeUserPassword(const string & NewPassword, string & Password);     ///< 更改密码
    double GetShareRate(){return UserShareRate;}
    int GetDownloads(){return UserDownloads;}
    int GetUploads(){return UserUploads;}
    string GetName(){return UserName;}
    int UpdateDownloads(int const DeltaD);
    int UpdateUploads(int const DeltaU);
    int UpdateCoins(int const DeltaC);
};