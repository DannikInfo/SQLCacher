#include <iostream>
#include <config.h>

#include <logger.h>
#include <set>
#include "configPathes.h"
#include "cacheThread.h"

using namespace std;

int main() {
    //Initialization logging
    logger::init("SQLCache");
    thread::id mainThreadID = this_thread::get_id();
    logger::setThread("MainThread", mainThreadID);

    //Initialization configuration
    config::init("SQLCache");
    config::set(DB_URL, "localhost");
    config::set(DB_USER, "root");
    config::set(DB_PASS, "root");
    config::set(DB_NAME, "ai");
    config::set(DEBUG, false);

    set<string> cachePaths;
    cachePaths.insert("30:/home/user/AIServer/cache");

    config::set(CACHE_PATHS, cachePaths);
    config::loadConfig();

    //Start cache task
    thread sqlT(cacheThread::run);

    string s;
    cin >> s;
}