cmake_minimum_required(VERSION 3.10)

project(classroombot VERSION 1.0)

file(GLOB_RECURSE SRC
    "src/*.cpp"
)

add_executable(classroombot ${SRC})

# set(CMAKE_BUILD_TYPE Debug)

set_property(TARGET classroombot PROPERTY CXX_STANDARD 17)

target_compile_definitions(classroombot PUBLIC STATIC_CONCPP AEGIS_SEPARATE_COMPILATION)

target_link_libraries(classroombot aegis pqxx pq pthread ssl crypto z fmt)

target_include_directories(classroombot PRIVATE "/usr/local/include")