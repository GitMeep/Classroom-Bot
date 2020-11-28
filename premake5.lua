workspace "classroombot"
    configurations {"Debug", "Release"}

project "classroombot"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin"
    cppdialect "C++17"
    toolset "gcc"

    linkoptions { '-static-libstdc++', '-static-libgcc' }
    enablewarnings { "invalid-pch" }
    files { "./src/**.h", "./src/**.cpp" }
    pchheader "./src/cbpch.h"
    defines { "BOT_VERSION=\"1.8\"", "AEGIS_SEPARATE_COMPILATION" }
    includedirs { 
        "/usr/local/include/",
        "/usr/local/include/mongocxx/v_noabi",
        "/usr/local/include/bsoncxx/v_noabi",
        "/usr/local/include/libmongoc-1.0",
        "/usr/local/include/libbson-1.0",
        "/usr/include/",
        "./src/"
    }
    links {
        "pthread",
        "icuuc:static",
        "ssl",
        "crypto",
        "cryptopp",
        "z",
        "fmt",
        "restclient-cpp:static",
        "curl",
        "bsoncxx",
        "mongocxx"
    }

    filter "configurations:Debug"
        buildoptions { "-rdynamic" }
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"