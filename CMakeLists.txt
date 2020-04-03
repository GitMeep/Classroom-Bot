cmake_minimum_required(VERSION 3.10)

project(classroombot VERSION 1.0)

file(GLOB_RECURSE commands_SRC
    "src/*.cpp"
)

add_executable(classroombot src/main.cpp src/bot/bot.cpp src/bot/utils/utils.cpp ${commands_SRC})

# set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(classroombot pthread ssl crypto z fmt)

target_include_directories(classroombot PUBLIC "/usr/local/include")