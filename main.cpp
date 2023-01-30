#include <iostream>
#include <config.h>

#include <logger.h>
#include <list>
#include "configPathes.h"
#include "cacheThread.h"

int main() {
    logger::init("SQLCache");
    std::thread::id mainThreadID = std::this_thread::get_id();
    logger::setThread("MainThread", mainThreadID);

    config::init("SQLCache");
    config::set(DB_URL, "localhost");
    config::set(DB_USER, "root");
    config::set(DB_PASS, "root");
    config::set(DB_NAME, "ai");
    config::set(DEBUG, false);

    std::list<std::string> cachePaths;
    cachePaths.emplace_back("30:/home/user/AIServer/cache");

    config::set(CACHE_PATHS, cachePaths);
    config::loadConfig();

    std::thread sqlT(cacheThread::run);

    std::string s;
    std::cin >> s;
}