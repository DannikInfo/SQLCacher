#include <iostream>
#include <config.h>

#include <logger.h>
#include <set>

#include <metida/core/core.h>
#include <metida/core/redis/heartBeatThread.h>

#include "configPathes.h"
#include "cacheThread.h"

#define WITH_DB true
#define WITH_DNN false
#define WITH_S3 false
#define WITH_REDIS true

using namespace std;

int main() {
    //Initialization logging
    logger::init("SQLCache");
    thread::id mainThreadID = this_thread::get_id();
    logger::setThread("MainThread", mainThreadID);

    //Initialization configuration
    config::init("SQLCache");
    core::init(WITH_DB, WITH_DNN, WITH_S3, WITH_REDIS);

    config::set(CLIENT_ID, 2);
    config::set(DEBUG, false);

    set<string> cachePaths;
    cachePaths.insert("30:/home/user/AIServer/cache");

    config::set(CACHE_PATHS, cachePaths);
    config::loadConfig();

    //Start cache task
    thread sqlT(cacheThread::run);
    thread heartBeatT(heartBeatThread::run, "SQLCache-"+std::to_string(config::get<int>(CLIENT_ID)));

    string s;
    cin >> s;
}