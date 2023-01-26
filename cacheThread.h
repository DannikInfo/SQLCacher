#ifndef SQLCACHE_CACHETHREAD_H
#define SQLCACHE_CACHETHREAD_H

#include <list>
#include <config.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <thread>
#include <logger.h>
#include <utils.h>
#include <mutex>

#include "configPathes.h"
#include "SQLManager.h"
class cacheThread : public std::thread{
private:
    static std::mutex m;
public:
    static void getFilesRecursive(const std::filesystem::path &path, std::list<std::string> &SQLs);

    static void run(const std::string &path, int num, int delay);
};


#endif //SQLCACHE_CACHETHREAD_H
