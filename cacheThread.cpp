#include "cacheThread.h"

time_t curr_time;
tm * curr_tm;
char dt[50];

std::mutex cacheThread::m;

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
                if (std::filesystem::is_empty(p) && !strstr(p.path().c_str(), dt))
                    std::filesystem::remove(p);
            }
        }
    }catch (std::exception &ex){
        logger::error(ex.what());
    }
}

void cacheThread::run(const std::string &path, int num, int delay){
    std::thread::id threadID = std::this_thread::get_id();
    logger::setThread("CacheThread-"+std::to_string(num), threadID);

    std::string query;
    std::list<std::string> SQLs;

    while(true){
        SQLs.clear();

        logger::info("Start check SQL cache");
        getFilesRecursive("cache", SQLs);

        if(!SQLs.empty()) {
            logger::info("Found SQL cache, start process upload..");
            for (const auto &item: SQLs) {
                SQLManager::renovateConnection();

                std::ifstream f(item, std::ios::in);
                if(f.is_open()) {
                    query = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                    m.lock();
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

                    std::filesystem::copy_file(item, "cache/bad/"+vecStr[vecStr.size()-1]);
                }

                std::filesystem::remove(item);
            }
            logger::info("SQL cache handled, sleep for "+std::to_string(delay) + "sec.");
        }else {
            logger::info("SQL cache not found, sleep for "+std::to_string(delay) + "sec.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}