#include "include/sqlite_3.H"
#include <sqlite3.h>

namespace abxhttpd {

ModuleConfigure sqlite_3_info={"sqlite3",{
    {"Support","enabled"},
    {"SQLite Version",sqlite3_version},
    {"Header Wrapper","Abxhttpd SQLite3 C++ Header Wrapper 1.0"},
    }};
Module sqlite_3_module(&sqlite_3_info);

sqlite_3_stmt::sqlite_3_stmt(sqlite3_stmt * pstmt){
    stmt=pstmt;
}

void sqlite_3_stmt::reset(){
    sqlite3_reset(stmt);
}

void sqlite_3_stmt::bind(int r,int src){
    sqlite3_bind_int(stmt, r, src);
}

void sqlite_3_stmt::bind(int r,long src){
    sqlite3_bind_int64(stmt, r, src);
}

void sqlite_3_stmt::bind(int r,const char * src){
    sqlite3_bind_text(stmt, r, src, strlen(src), NULL);
}

int sqlite_3_stmt::step(){
    return sqlite3_step(stmt);
}

sqlite_3_stmt::~sqlite_3_stmt(){
    if(stmt!=NULL){
        sqlite3_finalize(stmt);
    }
}

const unsigned char * sqlite_3_stmt::row(int row){
    return sqlite3_column_text(stmt, row);
}

int sqlite_3_stmt::row(int row,const char ** dst){
    *dst=(const char *)sqlite3_column_text(stmt, row);
    return 0;
}

int sqlite_3_stmt::row(int row,int * dst){
    *dst=sqlite3_column_int(stmt , row);
    return 0;
}

int sqlite_3_stmt::row(int row,long * dst){
    *dst=sqlite3_column_int64(stmt , row);
    return 0;
}

sqlite_3::sqlite_3(const char * path){
    status_id=sqlite3_open(path, &database);
}

sqlite_3_stmt * sqlite_3::prepare(const char * sql){
    sqlite3_stmt * tmp;
    int st=sqlite3_prepare_v2(database, sql, -1, &tmp, NULL);
    if(st==SQLITE_OK){
        return (new sqlite_3_stmt(tmp));
    }
    return NULL;
}

sqlite_3::~sqlite_3(){
    sqlite3_close(database);
}

}