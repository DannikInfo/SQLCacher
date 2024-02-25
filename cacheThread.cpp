#include "cacheThread.h"

time_t curr_time;
tm * curr_tm;
char dt[50], dtt[50];
int maxTimer;

//Search all .sql files in path
void cacheThread::getFilesRecursive(const filesystem::path& path, set<string>& SQLs) {
    if(!filesystem::exists(path)) {
        logger::error(path.string() + " not exists!");
        return;
    }

    time(&curr_time);
    curr_tm = localtime(&curr_time);
    bzero(dt, 50);
    strftime(dt, 50, "%d-%m-%Y", curr_tm);

    try {
        for (const auto &p: filesystem::recursive_directory_iterator(path)) {
            if (!is_directory(p) && !strstr(p.path().c_str(), "bad")) { //if path is directory and not contains bad
                string sql = p.path();
                if (sql.substr(sql.length() - 4, sql.length()) == ".sql") { //if path contains .sql extension, add to set
                    if (DEBUG)
                        logger::info("Found sql file: " + sql);
                    SQLs.insert(sql);
                } else {
                    if (DEBUG)
                        logger::info("Found non sql file: " + sql);
                    if(!badProcessing(sql))
                        continue;
                }
            } else {
                if (filesystem::is_empty(p.path()) && !strstr(p.path().c_str(), dt)) //remove old xx_xx_xxxx dirs from cache directory
                    filesystem::remove(p.path());
            }
        }
    }catch (exception &ex){
        logger::error(ex.what());
    }
}

//Parsing delay:path from config
void cacheThread::parseCachePaths(const set<string> &listCachesStr, vector<cacheThread::cache> &caches){
    vector<string> vecToken;
    maxTimer = 0;
    for (const auto &item: listCachesStr){
        vecToken.clear();
        utils::tokenize(item, ":", vecToken);

        cacheThread::cache c;

        c.delay = stoi(vecToken[0]);
        if(c.delay > maxTimer)
            maxTimer = c.delay;

        c.path = vecToken[1];
        caches.emplace_back(c);
    }
}

//Main thread method
void cacheThread::run(){
    //Initialize log thread naming
    thread::id cThreadID = this_thread::get_id();
    logger::setThread("CacheThread", cThreadID);

    //Parsing paths
    vector<cacheThread::cache> vecCache;
    parseCachePaths(config::get<set<string>>(CACHE_PATHS), vecCache);

    string query;
    set<string> SQLs;
    int timer = 0;

    //Start infinity loop
    while(true){
        SQLs.clear();
        if(timer > maxTimer)
            timer = 0;

        for (const auto &item: vecCache){ //Checking if the timer value matches the specified delay. 1, 0 - excluded for
            if(timer % item.delay == 0 && timer != 1 && timer != 0 || item.delay == 1) //fix false positives, if delay 1, it will run every cycle
                getFilesRecursive(item.path, SQLs);
        }

        if(!SQLs.empty()) { //if founded .sql files in paths
            logger::info("Found SQL cache, start process upload..");
            int delayCounter = 0;
            for (const auto &item: SQLs) {
                SQLManager::renovateConnection();

                ifstream f(item, ios::in);
                if(f.is_open()) { //try open and read .sql
                    query = string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                    if(DEBUG) logger::success("File " + item + " was open and read");
                }else{
                    logger::error("File "+item+" failed to open!");
                    continue;
                }
                f.close();

                try{ //try to execute SQL from file
                    logger::success("File " +item+ " try execute with SQL "+ query);
                    SQLManager::getConnection()->createStatement()->execute(query);
                }catch(sql::SQLException &ex){ //if execution failed, copy file to ./cache/bad
                    logger::error("File " +item+ " error on execute with SQL "+ query +" ERROR: "+ ex.what());
                    if(!badProcessing(item))
                        continue;
                }
                filesystem::remove(item); //remove file after handling
                delayCounter++;

                if(delayCounter > 100){
                    this_thread::sleep_for(chrono::milliseconds(100));
                    delayCounter = 0;
                }
            }
            logger::info("SQL cache handled, waiting...");
        }

        timer++;
        this_thread::sleep_for(chrono::milliseconds(1000)); //1sec sleep for timer
    }
}

bool cacheThread::badProcessing(const string& SQLPath){
    time(&curr_time);
    curr_tm = localtime(&curr_time);
    bzero(dtt, 50);
    strftime(dtt, 50, "%d-%m-%Y %H_%M_%S", curr_tm);

    if(!filesystem::exists("cache/bad"))
        filesystem::create_directories("cache/bad");

    const string& filePath = SQLPath;
    vector<string> vecStr;
    utils::tokenize(filePath, "/", vecStr);
    try { //try to copy file
        if(!filesystem::exists("cache/bad/" + vecStr[vecStr.size() - 2]))
            filesystem::create_directories("cache/bad/" + vecStr[vecStr.size() - 2]);
        filesystem::copy_file(SQLPath, "cache/bad/" + vecStr[vecStr.size() - 2] + "/" + vecStr[vecStr.size() - 1] + "_" + dtt);
        filesystem::remove(SQLPath);
    }catch(exception &ex2){
        logger::error("Copy " +SQLPath+ " ERROR: "+ ex2.what());
        return false;
    }
    return true;
}