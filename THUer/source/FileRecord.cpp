#include "../include/FileRecord.h"
#include <iostream>
#include <cstring>
#include <abxhttpd/sqlite_3.H>
#include <configor/json.hpp>

using namespace std;
using abxhttpd::sqlite_3;
using abxhttpd::sqlite_3_stmt;
using configor::json;

sqlite_3 dta("./thuer.db");

sqlite_3_stmt * FileRecord::_SearchFile(int PageSize, int Page, const string & filename)
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
    return dta_file;
}

sqlite_3_stmt * FileRecord::_ShowFile(int PageSize, int Page)
{
    auto dta_file = dta.prepare("SELECT TITLE,INFO,SHARER,SIZE,DOWNLOAD,PATH FROM F_INFO LIMIT ?,?");
    dta_file->bind(1,Page*PageSize);
    dta_file->bind(2,PageSize);
    return dta_file;
}

sqlite_3_stmt * FileRecord::_HotFile(int PageSize, int Page)
{
    auto dta_file = dta.prepare("SELECT TITLE,INFO,SHARER,SIZE,DOWNLOAD, PATH  FROM F_INFO WHERE TRUE ORDER BY DOWNLOAD DESC LIMIT ?, ?");
    dta_file->bind(1,Page*PageSize);
    dta_file->bind(2,PageSize);
    return dta_file;
}  

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


string FileRecord::SearchFile(int PageSize, int Page, const string & filename)
{
    return ConstructJSON(_SearchFile(PageSize, Page, filename));
}

string FileRecord::ShowFile(int PageSize, int Page)
{
    return ConstructJSON(_ShowFile(PageSize, Page));
}

string FileRecord::HotFile(int PageSize, int Page)
{
    return ConstructJSON(_HotFile(PageSize, Page));
}