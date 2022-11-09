#include <cstring>

enum Rank{peasent = 1, elite, crazy, admin};

class User{
private:
    int Grade;

    static bool CheckIfAlready(const string & Username);
public:
    User(const string & Username){}
    User(){}
    ~User(){}
    static bool RegisterNewUser(const string & Username, string & password);
    static bool Verify(const string & Username, string & password);

};