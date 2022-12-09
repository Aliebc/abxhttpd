#ifndef FILE_RECORD_H
#define FILE_RECORD_H

#include <iostream>
#include <cstring>
#include <abxhttpd/sqlite_3.H>
#include <configor/json.hpp>
#include "../include/FileRecord.h"
using namespace std;
using abxhttpd::sqlite_3;
using abxhttpd::sqlite_3_stmt;
using configor::json;


class FileRecord{
private:
    static sqlite_3_stmt * _SearchFile(int PageSize, int Page, const string & filename);
    static sqlite_3_stmt * _ShowFile(int PageSize, int Page); 
    static sqlite_3_stmt * _HotFile(int PageSize, int Page);    
    static string ConstructJSON(sqlite_3_stmt * dta);
public:
    static string SearchFile(int PageSize, int Page, const string & filename);
    static string ShowFile(int PageSize, int Page); 
    static string HotFile(int PageSize, int Page); 
};
#endif