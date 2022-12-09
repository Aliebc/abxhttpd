#include "../include/FileRecord.h"
#include <iostream>
#include <cstring>
#include <abxhttpd/sqlite_3.H>
#include <configor/json.hpp>

using namespace std;
using abxhttpd::sqlite_3;
using abxhttpd::sqlite_3_stmt;
using configor::json;

sqlite_3 dta("./thuer.db"); ///< 数据库，保存了用户信息和文件信息


/**
 * @brief 构建JSON格式字符串
 * 
 * 将sqlite_3_stmt指针里的内容构建为JSON格式字符串
 * @param dta 包含文件信息的sqlite_3_stmt指针
 * @return string 包含文件信息的JSON格式字符串
 */
string FileRecord::ConstructJSON(sqlite_3_stmt * dta)
{
    json res;
    if (dta == NULL)
    {
        res["code"] = -1;
        res["errMsg"] = "NULL";
        return res.dump();
    }
    res["code"] = 0;
    res["data"] = json::array({});
    int i = 0;
    while (dta->step() == SQLITE_ROW)
    {
        json tmp;
        tmp["title"] = string((const char *)dta -> row(0));
        tmp["info"] = string((const char *)dta -> row(1));
        int sharer_download;
        dta -> row(2, &sharer_download);
        tmp["sharer"] = sharer_download;
        long size;
        dta -> row(3,&size);
        tmp["size"] = size;
        dta -> row(4, &sharer_download);
        tmp["download"] = sharer_download;
        if (dta -> row(5) == NULL)
            tmp["path"] = " ";
        else
            tmp["path"] = string((const char *)dta -> row(5));
        res["data"][i++] = tmp;
    }
    res["count"] = i;
    return res.dump(4);
}


/**
 * @brief 查找文件
 * 
 * 查找特定名称的文件，根据指定页面大小和当前页码，返回对应的JSON格式字符串
 * @param PageSize 页面大小
 * @param Page 当前页码
 * @param filename 文件名称
 * @return string 包含文件信息的JSON格式字符串
 */
string FileRecord::SearchFile(int PageSize, int Page, const string & filename)
{
    if (filename == "")
        return NULL;
    
    string cmd = "SELECT TITLE,INFO,SHARER,SIZE,DOWNLOAD, PATH FROM F_INFO WHERE TITLE LIKE '%" 
                + filename 
                + "%' LIMIT "
                + to_string(Page * PageSize)
                + " , "
                + to_string(PageSize);
    sqlite_3_stmt *dta_file = (dta.prepare(cmd.c_str()));
    return ConstructJSON(dta_file);
}


/**
 * @brief 展示所有文件
 * 
 * 展示所有文件，根据指定页面大小和当前页码，返回对应的JSON格式字符串
 * @param PageSize 页面大小
 * @param Page 当前页码
 * @return string 包含文件信息的JSON格式字符串
 */
string FileRecord::ShowFile(int PageSize, int Page)
{
    auto dta_file = dta.prepare("SELECT TITLE,INFO,SHARER,SIZE,DOWNLOAD,PATH FROM F_INFO LIMIT ?,?");
    dta_file->bind(1,Page*PageSize);
    dta_file->bind(2,PageSize);
    return ConstructJSON(dta_file);
}


/**
 * @brief 展示热榜文件
 * 
 * 根据下载量降序排列文件，展示热榜文件，根据指定页面大小和当前页码，返回对应的JSON格式字符串
 * @param PageSize 页面大小
 * @param Page 当前页码
 * @return string 包含文件信息的JSON格式字符串
 */
string FileRecord::HotFile(int PageSize, int Page)
{
    auto dta_file = dta.prepare("SELECT TITLE,INFO,SHARER,SIZE,DOWNLOAD, PATH  FROM F_INFO WHERE TRUE ORDER BY DOWNLOAD DESC LIMIT ?, ?");
    dta_file->bind(1,Page*PageSize);
    dta_file->bind(2,PageSize);
    return ConstructJSON(dta_file);
}