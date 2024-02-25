cmake_minimum_required(VERSION 3.22)
project(SQLCache)

find_package(Threads REQUIRED)
set(CMAKE_CXX_STANDARD 23)

add_executable(SQLCache main.cpp cacheThread.cpp cacheThread.h configPathes.h SQLManager.cpp SQLManager.h)

target_link_libraries(${PROJECT_NAME} mysqlcppconn)
target_link_libraries(${PROJECT_NAME} libmetidaCore.so)
target_link_libraries(${PROJECT_NAME} libredis++.so)
target_link_libraries(${PROJECT_NAME} libhiredis.so)
target_link_libraries(${PROJECT_NAME} libtinyConfig.so libtinyLogger.so libtinyUtils.so)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
