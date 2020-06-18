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
    files { "./src/**.h", "./src/**.cpp", "./src/**.cxx" }
    pchheader "./src/cbpch.h"
    defines { "BOT_VERSION=\"1.7\"", "AEGIS_SEPARATE_COMPILATION" }
    includedirs { "/usr/local/include/", "./src/" }
    links {
        "pthread",
        "ssl",
        "crypto",
        "z",
        "fmt",
        "restclient-cpp",
        "curl",
        "pqxx",
        "pq"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"