project "ImGuiTestEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "imgui_test_engine/**.cpp",
        "imgui_test_engine/**.h"
    }

    includedirs {
        "imgui_test_engine",
        "../imgui"
    }

    defines {
        "IMGUI_ENABLE_TEST_ENGINE",
        "IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        optimize "off"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
