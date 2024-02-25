#ifndef SQLCACHE_CACHETHREAD_H
#define SQLCACHE_CACHETHREAD_H

#include <set>
#include <vector>
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

using namespace std;

class cacheThread : public thread{
public:
    struct cache{
        int delay;
        string path;
    };

    static void getFilesRecursive(const filesystem::path &path, set<string> &SQLs);

    static void run();

    static void parseCachePaths(const set<string> &listCachesStr, vector<cacheThread::cache> &caches);

    static bool badProcessing(const string &SQLPath);
};


#endif //SQLCACHE_CACHETHREAD_H
