#include "SQLManager.h"

bool SQLManager::connected = false;
sql::Driver* SQLManager::driver = get_driver_instance();
sql::Connection* SQLManager::con;

void SQLManager::renovateConnection(){
    connected = con != nullptr && (con->isValid() || con->reconnect());
    while(!connected) {
        try {
            con = driver->connect(config::get<std::string>(DB_URL), config::get<std::string>(DB_USER),config::get<std::string>(DB_PASS));
            con->setSchema(config::get<std::string>(DB_NAME));
            connected = con != nullptr && (con->isValid() || con->reconnect());
        } catch (sql::SQLException &ex) {
            logger::warn("Connection to server is failed, try reconnecting in 5sec");
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
    }
}

sql::Driver *SQLManager::getDriver() {
    return driver;
}

sql::Connection *SQLManager::getConnection() {
    return con;
}

bool SQLManager::isConnected() {
    return connected;
}
