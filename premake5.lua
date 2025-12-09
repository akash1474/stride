workspace "StrideProject"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDirs={}
includeDirs["glfw"]="packages/glfw/include"
includeDirs["SpdLog"]="packages/spdlog/include"
includeDirs["ImGui"]="packages/imgui"
includeDirs["LunaSVG"]="packages/lunasvg/include"
includeDirs["nlohmann"]="packages/nlohmann/include"
includeDirs["ImAnim"]="packages/ImAnim/src"
includeDirs["freetype"]="packages/freetype/include"
includeDirs["ImGuiTestEngine"]="packages/imgui_test_engine"
includeDirs["SQLite"]="packages/sqlite"

include "packages/glfw"
include "packages/imgui"
include "packages/lunasvg"
include "packages/ImAnim"
include "packages/imgui_test_engine"
include "packages/sqlite"

project "Stride"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin"
    objdir "bin/obj"
    pchheader "pch.h"
    pchsource "src/pch.cpp"
    staticruntime "On"

    links {
        "glfw","ImGui","opengl32","LunaSVG","dwmapi","Shlwapi","winmm","ImAnim","freetype", "ImGuiTestEngine","SQLite"
    }

    includedirs{
        "src",
        "src/external",
        "%{includeDirs.glfw}",
        "%{includeDirs.ImGui}",
        "%{includeDirs.LunaSVG}",
        "%{includeDirs.SpdLog}",
        "%{includeDirs.nlohmann}",
        "%{includeDirs.ImAnim}",
        "%{includeDirs.freetype}",
        "%{includeDirs.ImGuiTestEngine}",
        "%{includeDirs.SQLite}"
    }

    files { 
        "src/**.cpp"
    }


    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        buildoptions { "/MP","/DEBUG:FULL","/utf-8" }
        buildoptions { "/MP","/DEBUG:FULL","/utf-8" }
        defines {"GL_DEBUG", "IMGUI_ENABLE_TEST_ENGINE", "IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL"}

    filter {"configurations:Release"}
        runtime "Release"
        optimize "On"
        symbols "Off"
        characterset ("MBCS")
        buildoptions { "/MP","/utf-8" }
        buildoptions { "/MP","/utf-8" }
        defines {"GL_DEBUG","_CRT_SECURE_NO_WARNINGS", "IMGUI_ENABLE_TEST_ENGINE", "IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL"}

    filter "configurations:Dist"
        kind "WindowedApp"
        runtime "Release"
        optimize "On"
        symbols "Off"
        characterset ("MBCS")
        buildoptions { "/MP","/utf-8"}
        linkoptions {"/ENTRY:mainCRTStartup"}
