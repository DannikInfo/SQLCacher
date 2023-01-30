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

#include "configPathes.h"
#include "SQLManager.h"
class cacheThread : public std::thread{
public:
    struct cache{
        int delay;
        std::string path;
    };

    static void getFilesRecursive(const std::filesystem::path &path, std::list<std::string> &SQLs);

    static void run();

    static void parseCachePaths(const std::list<std::string> &listCachesStr, std::vector<cacheThread::cache> &caches);
};


#endif //SQLCACHE_CACHETHREAD_H
