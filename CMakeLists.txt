cmake_minimum_required(VERSION 3.23)
project(SQLCache)

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 23)

add_executable(SQLCache main.cpp cacheThread.cpp cacheThread.h configPathes.h SQLManager.cpp SQLManager.h)

target_link_libraries(${PROJECT_NAME} mysqlcppconn)
target_link_libraries(${PROJECT_NAME} libtinyConfig.so libtinyLogger.so libtinyUtils.so)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
