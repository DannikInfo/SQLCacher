#ifndef SQLCACHE_SQLMANAGER_H
#define SQLCACHE_SQLMANAGER_H

#include <list>
#include <config.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <thread>
#include <logger.h>

#include "configPathes.h"

using namespace std;

class SQLManager {
private:
    static bool connected;
    static sql::Driver* driver;
    static sql::Connection* con;

public:
    static void renovateConnection();

    static sql::Driver* getDriver();
    static sql::Connection* getConnection();
    static bool isConnected();
};

#endif //SQLCACHE_SQLMANAGER_H
