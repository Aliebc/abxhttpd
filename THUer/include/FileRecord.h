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

/**
 * @brief 文件记录类
 * 
 * 这个类是函数的集合，函数的功能都是在数据库中搜索文件，然后以特定的方式返回 
 */
class FileRecord{
private:
    static string ConstructJSON(sqlite_3_stmt * dta);                                    ///< 将sqlite_3_stmt指针内容打包为JSON格式的string并返回
public:
    static string SearchFile(int PageSize, int Page, const string & filename);           ///< 搜索某个名字的文件，返回JSON格式的string
    static string ShowFile(int PageSize, int Page);                                      ///< 展示所有文件，返回JSON格式的string
    static string HotFile(int PageSize, int Page);                                       ///< 展示文件下载量热榜，返回JSON格式的string
};
#endif