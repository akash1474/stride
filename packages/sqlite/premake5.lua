project "SQLite"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "**.c",
        "**.h",
    }

    includedirs
    {
        "."
    }

    defines
    {
        "SQLITE_ENABLE_COLUMN_METADATA",
        "SQLITE_ENABLE_FTS5",
        "SQLITE_ENABLE_RTREE",
        "SQLITE_THREADSAFE=1"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        optimize "off"
        buildoptions { "/MP" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        buildoptions { "/MP" }

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
        buildoptions { "/MP" }