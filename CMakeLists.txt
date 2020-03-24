cmake_minimum_required(VERSION 3.10)

project(questionsbot VERSION 1.0)

add_executable(questionsbot main.cpp)

target_link_libraries(questionsbot pthread ssl crypto z)

target_include_directories(questionsbot PUBLIC "/usr/local/include")