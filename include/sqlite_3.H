#ifndef SQLITE_3_ABX_H
#define SQLITE_3_ABX_H
#include <sqlite3.h>
#include "Module.hxx"
#include <cstddef>
#include <cstring>
#include <memory>

namespace abxhttpd {

/**
 * @brief SQLite3语句类
 * 
 */
class ABXHTTPD_API sqlite_3_stmt
{
private:
    sqlite3_stmt * stmt;
public:
    sqlite_3_stmt(sqlite3_stmt *);
    ~sqlite_3_stmt();
    int step();
    void bind(int,int);
    void bind(int,long);
    void bind(int,const char *);
    const unsigned char * row(int);
    int row(int,int *);
    int row(int,long *);
    int row(int,float *);
    int row(int, const char **);
    void reset();
};
/**
 * @brief SQLite3数据库拓展模块
 * 
 * 提供一个C->C++的转写,用C++类包装一些基础SQL操作, 用于底层数据库的拓展
 */
class ABXHTTPD_API sqlite_3
{
private:
    int status_id;
    sqlite3 * database;
public:
    sqlite_3(const char * path);
    sqlite_3_stmt * prepare(const char * sql);
    ~sqlite_3();
    int status() const;
};

}
#endif