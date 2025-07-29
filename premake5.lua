require ".premake/ecc/ecc"

workspace "GameTest"
    configurations { "Debug", "Release" }
    location "build"

project "GameTest"
    kind "ConsoleApp"
    language "C++"
    files { "src/**.hpp", "src/**.cpp" }
    location "build"
    includedirs { "src" }
    cppdialect "C++23"

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"