#ifndef DATABASE_H
#define DATABASE_H
#include <sqlite3.h>
#include <cstddef>
#include <cstring>

class sqlite_3_stmt
{
private:
    sqlite3_stmt * stmt;
public:
    sqlite_3_stmt(sqlite3_stmt *);
    ~sqlite_3_stmt();
    int step();
    int bind(int,int);
    int bind(int,float);
    void bind(int,const char *);
    const unsigned char * row(int);
    int row(int,int *);
    int row(int,long *);
    int row(int,float *);
    int row(int, const char **);
    void reset();
};

inline sqlite_3_stmt::sqlite_3_stmt(sqlite3_stmt * pstmt){
    stmt=pstmt;
}

inline void sqlite_3_stmt::bind(int r,const char * src){
    sqlite3_bind_text(stmt, r, src, strlen(src), NULL);
}

inline int sqlite_3_stmt::step(){
    return sqlite3_step(stmt);
}

inline sqlite_3_stmt::~sqlite_3_stmt(){
    if(stmt!=NULL){
        sqlite3_finalize(stmt);
    }
}

inline const unsigned char * sqlite_3_stmt::row(int row){
    return sqlite3_column_text(stmt, row);
}

inline int sqlite_3_stmt::row(int row,const char ** dst){
    *dst=(const char *)sqlite3_column_text(stmt, row);
    return 0;
}

inline int sqlite_3_stmt::row(int row,int * dst){
    *dst=sqlite3_column_int(stmt , row);
    return 0;
}

inline int sqlite_3_stmt::row(int row,long * dst){
    *dst=sqlite3_column_int64(stmt , row);
    return 0;
}

class sqlite_3
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

inline sqlite_3::sqlite_3(const char * path){
    status_id=sqlite3_open(path, &database);
}

inline sqlite_3_stmt * sqlite_3::prepare(const char * sql){
    sqlite3_stmt * tmp;
    int st=sqlite3_prepare_v2(database, sql, -1, &tmp, NULL);
    if(st==SQLITE_OK){
        return (new sqlite_3_stmt(tmp));
    }
    return NULL;
}

inline sqlite_3::~sqlite_3(){
    sqlite3_close(database);
}
#endif