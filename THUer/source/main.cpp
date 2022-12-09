#include "../include/FileRecord.h"
#include "../include/user.h"
#include <iostream>
#include <abxhttpd/sqlite_3.H>
#include <configor/json.hpp>
using namespace std;
using abxhttpd::sqlite_3;
using abxhttpd::sqlite_3_stmt;
using configor::json;

int main()
{
    // sqlite_3_stmt * dta = FileRecord::ShowFile(20, 20);
    // cout << FileRecord::ConstructJSON(dta);


    User test;
    cout << test.UploadFile("helloworld", "hh", "ww") << endl;
    if (User::RegisterNewUser("TTDiang", "123456"))
        cout << "true" << endl;
    else
        cout << "false" << endl;
    if (User::RegisterNewUser("TTDiang2", "123456"))
        cout << "true" << endl;
    else
        cout << "false" << endl;
    if (User::RegisterNewUser("X123", "123"))
        cout << "true" << endl;
    else
        cout << "false" << endl;
    if (User::RegisterNewUser("X456", "456"))
        cout << "true" << endl;
    else
        cout << "false" << endl;

    // sqlite_3 base("./thuer.db");
    // auto k=base.prepare("INSERT INTO F_INFO (TITLE,INFO,SHARER) VALUES (?,?,?)");
    // for (int i=0;i<1000;i++){
    //     string fname="File "+to_string(i);
    //     if(k!=NULL){
    //         k->bind(1,fname.c_str());
    //         k->bind(2,("This is "+fname).c_str());
    //         k->bind(3,1);
    //         k->step();
    //         k->reset();
    //     }else{
    //         cout << "NULL" <<endl;
    //     }
    // }
    return 0;
}