cmake_minimum_required(VERSION 2.7)
project(monrc)

add_definitions(-std=c++11)

include_directories(include)

add_subdirectory(src)

install(FILES monrc_config.xml DESTINATION etc)
