cmake_minimum_required(VERSION 3.10)

project(questionsbot VERSION 1.0)

add_executable(questionsbot src/main.cpp src/bot/bot.cpp src/bot/commands/helpCommand.cpp src/bot/commands/questionCommand.cpp src/bot/utils/utils.cpp)

set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(questionsbot pthread ssl crypto z fmt)

target_include_directories(questionsbot PUBLIC "/usr/local/include" ${CMAKE_CURRENT_SOURCE_DIR}/include)