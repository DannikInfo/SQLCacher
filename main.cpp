#include <iostream>
#include <config.h>
#include <utils.h>

#include <logger.h>
#include <list>
#include "configPathes.h"
#include "cacheThread.h"

std::list<std::thread> threads;

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

    cachePaths = config::get<std::list<std::string>>(CACHE_PATHS);

    std::vector<std::string> vecToken;
    int i = 0;
    for (const auto &item: cachePaths){
        utils::tokenize(item, ":", vecToken);
        threads.emplace_back(std::thread(cacheThread::run, vecToken[1], i, std::stoi(vecToken[0])));
        i++;
    }
    std::string s;
    std::cin >> s;
}