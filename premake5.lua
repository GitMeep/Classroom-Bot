workspace "classroombot"
    configurations {"Debug", "Release"}

project "classroombot"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin"
    cppdialect "C++17"
    toolset "gcc"

    linkoptions { '-static-libstdc++', '-static-libgcc' }
    files { "./src/**.h", "./src/**.cpp" }
    defines { "BOT_VERSION=\"2.0.1\"" }
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
        "curl",
        "bsoncxx",
        "mongocxx",
        "PocoFoundation",
        "PocoNet",
        "PocoNetSSL",
        "dpp"
    }

    filter "configurations:Debug"
        buildoptions { "-rdynamic" }
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"