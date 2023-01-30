#include "cacheThread.h"

time_t curr_time;
tm * curr_tm;
char dt[50];
int maxTimer;

void cacheThread::getFilesRecursive(const std::filesystem::path& path, std::list<std::string>& SQLs) {
    if(!std::filesystem::exists("cache")){
        std::filesystem::create_directory("cache");
        return;
    }

    time(&curr_time);
    curr_tm = localtime(&curr_time);
    bzero(dt, 50);
    strftime(dt, 50, "%d-%m-%Y", curr_tm);
    try {
        for (const auto &p: std::filesystem::recursive_directory_iterator(path)) {
            if (!std::filesystem::is_directory(p) && !strstr(p.path().c_str(), "bad")) {
                std::string sql = p.path();
                if (sql.substr(sql.length() - 4, sql.length()) == ".sql") {
                    if (DEBUG)
                        logger::info("Found sql file: " + sql);
                    SQLs.emplace_back(sql);
                } else {
                    if (DEBUG)
                        logger::info("Found non sql file: " + sql);
                }
            } else {
                if (std::filesystem::is_empty(p.path()) && !strstr(p.path().c_str(), dt))
                    std::filesystem::remove(p.path());
            }
        }
    }catch (std::exception &ex){
        logger::error(ex.what());
    }
}

void cacheThread::parseCachePaths(const std::list<std::string> &listCachesStr, std::vector<cacheThread::cache> &caches){
    std::vector<std::string> vecToken;
    maxTimer = 0;
    for (const auto &item: listCachesStr){
        vecToken.clear();
        utils::tokenize(item, ":", vecToken);
        cacheThread::cache c;
        c.delay = std::stoi(vecToken[0]);
        if(c.delay > maxTimer)
            maxTimer = c.delay;
        c.path = vecToken[1];
        caches.emplace_back(c);
    }
}

void cacheThread::run(){
    std::thread::id cThreadID = std::this_thread::get_id();
    logger::setThread("CacheThread", cThreadID);

    std::vector<cacheThread::cache> vecCache;
    parseCachePaths(config::get<std::list<std::string>>(CACHE_PATHS), vecCache);

    std::string query;
    std::list<std::string> SQLs;
    int timer = 0;
    while(true){
        SQLs.clear();
        if(timer > maxTimer)
            timer = 0;

        //logger::info("Start check SQL cache");
        for (const auto &item: vecCache){
            if(timer % item.delay == 0 && timer != 1 && timer != 0 || item.delay == 1)
                getFilesRecursive(item.path, SQLs);
        }

        if(!SQLs.empty()) {
            logger::info("Found SQL cache, start process upload..");
            for (const auto &item: SQLs) {
                SQLManager::renovateConnection();

                std::ifstream f(item, std::ios::in);
                if(f.is_open()) {
                    query = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                    if(DEBUG) logger::success("File " + item + " was open and read");
                }else{
                    logger::error("File "+item+" failed to open!");
                    continue;
                }
                f.close();

                try{
                    logger::success("File " +item+ " try execute with SQL "+ query);
                    SQLManager::getConnection()->createStatement()->execute(query);
                }catch(sql::SQLException &ex){
                    logger::error("File " +item+ " error on execute with SQL "+ query +" ERROR: "+ ex.what());
                    if(!std::filesystem::exists("cache/bad")) std::filesystem::create_directories("cache/bad");

                    std::vector<std::string> vecStr;
                    utils::tokenize(item, "/", vecStr);
                    logger::warn(vecStr[vecStr.size() - 1]);
                    try {
                        std::filesystem::copy_file(item, "cache/bad/" + vecStr[vecStr.size() - 1]);
                    }catch(std::exception &ex2){
                        logger::error("Copy " +item+ " ERROR: "+ ex.what());
                        continue;
                    }
                }
                std::filesystem::remove(item);
            }
            logger::info("SQL cache handled, waiting...");
        }

        timer++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}