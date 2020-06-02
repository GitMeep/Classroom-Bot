cmake_minimum_required(VERSION 3.10)

project(classroombot VERSION 1.6)

file(GLOB_RECURSE SRC
    "src/*.cpp"
)

add_executable(classroombot ${SRC})

set_property(TARGET classroombot PROPERTY CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-rdynamic")

target_compile_definitions(classroombot PUBLIC BOT_VERSION="${PROJECT_VERSION}")

target_link_libraries(classroombot pqxx pq pthread ssl crypto z fmt restclient-cpp.a curl)

target_include_directories(classroombot PRIVATE "/usr/local/include")