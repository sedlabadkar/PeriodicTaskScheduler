/*
 * db.h
 *
 *  Created on: Apr 24, 2017
 *      Author: sachin
 */

#ifndef DB_H_
#define DB_H_

#include <iostream>
#include <sqlite3.h>

class DB{
private:
    sqlite3 *db;
    int rc;
    DB(){
        rc = sqlite3_open("tasks.db", &db);
    };
    DB(DB const&)               = delete;
    void operator=(DB const&)  = delete;

public:

    double result;
    static DB& getInstance()
    {
        static DB instance;
        return instance;
    }

    bool execQuery(std::string query){
        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cout << sqlite3_errmsg(db) << endl;
            return false;
        }
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            result = sqlite3_column_double(stmt, 0);
        }
        if (rc != SQLITE_DONE) {
            cout << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
        return true;
    }
};


#endif /* DB_H_ */
