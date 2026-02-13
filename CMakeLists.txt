cmake_minimum_required(VERSION 3.25)

project(so-so LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "Supported configuration types")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if(MSVC)
    add_compile_options(/MP /permissive-)
    add_link_options(/ignore:4099)
endif()

add_compile_definitions(
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
    SPDLOG_USE_STD_FORMAT
    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

find_package(OpenGL REQUIRED)

# Vendor libraries
add_subdirectory(so-so/vendor/GLFW)

add_library(glad STATIC so-so/vendor/Glad/src/glad.c)
target_include_directories(glad PUBLIC so-so/vendor/Glad/include)

add_library(imgui STATIC
    so-so/vendor/imgui/imgui.cpp
    so-so/vendor/imgui/imgui_demo.cpp
    so-so/vendor/imgui/imgui_draw.cpp
    so-so/vendor/imgui/imgui_tables.cpp
    so-so/vendor/imgui/imgui_widgets.cpp
    so-so/vendor/imgui/imgui_stdlib.cpp
    so-so/vendor/imgui/backends/imgui_impl_glfw.cpp
    so-so/vendor/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC so-so/vendor/imgui so-so/vendor/imgui/backends)
target_link_libraries(imgui PUBLIC glfw glad)

add_library(TracyClient STATIC so-so/vendor/tracy/tracy/public/TracyClient.cpp)
target_include_directories(TracyClient PUBLIC so-so/vendor/tracy/tracy/public)
target_compile_definitions(TracyClient PUBLIC TRACY_ENABLE TRACY_ON_DEMAND TRACY_CALLSTACK=10)
target_link_libraries(TracyClient PUBLIC $<$<PLATFORM_ID:Windows>:Ws2_32 Dbghelp>)

# Project: so-so
add_library(so-so STATIC)
target_precompile_headers(so-so PRIVATE so-so/src/sspch.h)

file(GLOB_RECURSE SO_SO_SOURCES
    "so-so/src/**.h"
    "so-so/src/**.cpp"
    "so-so/vendor/glm/glm/**.hpp"
    "so-so/vendor/glm/glm/**.inl"
    "so-so/vendor/stb_image/stb_image.cpp"
    "so-so/vendor/spirv-cross/**.hpp"
    "so-so/vendor/spirv-cross/**.cpp"
)
target_sources(so-so PRIVATE ${SO_SO_SOURCES})

file(GLOB SPIRV_CROSS_SOURCES "so-so/vendor/spirv-cross/**.cpp")
set_source_files_properties(${SPIRV_CROSS_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

target_include_directories(so-so PRIVATE
    "so-so/src"
    "so-so/src/so-so/"
    "so-so/vendor/"
    "so-so/vendor/spdlog/include"
    "so-so/vendor/GLFW/include"
    "so-so/vendor/Glad/include"
    "so-so/vendor/imgui"
    "so-so/vendor/glm"
    "so-so/vendor/stb_image"
    "so-so/vendor/assimp/include"
    "so-so/vendor/shaderc/include"
    "so-so/vendor/spirv-cross"
    "so-so/vendor/tracy/tracy/public/tracy"
)

# Platform-specific vendored libraries
if(WIN32)
    # Windows uses static libs for assimp/shaderc (linked via Sandbox)
    target_link_libraries(so-so PUBLIC
        glfw
        glad
        imgui
        TracyClient
        OpenGL::GL
        dwmapi
    )
    target_compile_definitions(so-so PRIVATE SS_PLATFORM_WINDOWS GLFW_INCLUDE_NONE TRACY_ENABLE TRACY_ON_DEMAND TRACY_CALLSTACK=10)
elseif(UNIX AND NOT APPLE)
    # Linux uses vendored shared libraries
    add_library(assimp-vendor SHARED IMPORTED)
    set_target_properties(assimp-vendor PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/so-so/vendor/assimp/lib/linux/libassimp.so"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/so-so/vendor/assimp/include"
    )

    add_library(shaderc-vendor SHARED IMPORTED)
    set_target_properties(shaderc-vendor PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Debug/libshaderc_shared.so"
        IMPORTED_LOCATION_DEBUG "${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Debug/libshaderc_shared.so"
        IMPORTED_LOCATION_RELEASE "${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Release/libshaderc_shared.so"
        IMPORTED_LOCATION_DIST "${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Release/libshaderc_shared.so"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/include"
    )

    target_link_libraries(so-so PUBLIC
        glfw
        glad
        imgui
        TracyClient
        OpenGL::GL
        assimp-vendor
        shaderc-vendor
    )
    target_compile_definitions(so-so PRIVATE SS_PLATFORM_LINUX)
endif()

target_compile_definitions(so-so PRIVATE
    $<$<CONFIG:Debug>:SS_DEBUG>
    $<$<CONFIG:Release>:SS_RELEASE>
    $<$<CONFIG:Dist>:SS_DIST>
)

set_target_properties(so-so PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-x64/so-so"
)

# Project: Sandbox
add_executable(Sandbox)
file(GLOB_RECURSE SANDBOX_SOURCES "Sandbox/src/**.h" "Sandbox/src/**.cpp")
target_sources(Sandbox PRIVATE ${SANDBOX_SOURCES})

file(GLOB SHADER_FILES "Resources/Shader/**.glsl")
set_source_files_properties(${SHADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(Sandbox PRIVATE ${SHADER_FILES})

target_include_directories(Sandbox PRIVATE
    "Sandbox/src"
    "so-so/src"
    "so-so/vendor/spdlog/include"
    "so-so/vendor/GLFW/include"
    "so-so/vendor/Glad/include"
    "so-so/vendor/imgui"
    "so-so/vendor/glm"
    "so-so/vendor/stb_image"
    "so-so/vendor/tracy/tracy/public/tracy"
)

target_link_libraries(Sandbox PRIVATE so-so)

if(WIN32)
    target_compile_definitions(Sandbox PRIVATE SS_PLATFORM_WINDOWS)

    target_link_directories(Sandbox PRIVATE
        "${CMAKE_SOURCE_DIR}/so-so/vendor/assimp/lib/windows/$<CONFIG>"
        "${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/windows/$<CONFIG>"
    )

    target_link_libraries(Sandbox PRIVATE
        $<$<CONFIG:Debug>:assimp-vc143-MTd.lib shaderc-shared-MDd.lib>
        $<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:assimp-vc143-MT.lib shaderc-shared-MD.lib>
    )

    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/so-so/vendor/assimp/lib/windows/Debug/assimp-vc143-MTd.dll>"
            "$<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_SOURCE_DIR}/so-so/vendor/assimp/lib/windows/Release/assimp-vc143-MT.dll>"
            "$<TARGET_FILE_DIR:Sandbox>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/windows/Debug/shaderc-shared-MDd.dll>"
            "$<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/windows/Release/shaderc-shared-MD.dll>"
            "$<TARGET_FILE_DIR:Sandbox>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Sandbox/Resources"
            "$<TARGET_FILE_DIR:Sandbox>/Resources"
        COMMENT "Copying DLLs and resources to output directory"
    )
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(Sandbox PRIVATE SS_PLATFORM_LINUX)

    # Copy shared libraries to output directory (already linked via so-so library)
    add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/so-so/vendor/assimp/lib/linux/libassimp.so"
            "$<TARGET_FILE_DIR:Sandbox>"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "libassimp.so"
            "$<TARGET_FILE_DIR:Sandbox>/libassimp.so.5"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Debug/libshaderc_shared.so.1>$<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Release/libshaderc_shared.so.1>"
            "$<TARGET_FILE_DIR:Sandbox>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Debug/libshaderc_shared.so>$<$<OR:$<CONFIG:Release>,$<CONFIG:Dist>>:${CMAKE_SOURCE_DIR}/so-so/vendor/shaderc/lib/linux/Release/libshaderc_shared.so>"
            "$<TARGET_FILE_DIR:Sandbox>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Sandbox/Resources"
            "$<TARGET_FILE_DIR:Sandbox>/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Sandbox/assets"
            "$<TARGET_FILE_DIR:Sandbox>/assets"
        COMMENT "Copying shared libraries and resources to output directory"
    )
endif()

target_compile_definitions(Sandbox PRIVATE
    $<$<CONFIG:Debug>:SS_DEBUG>
    $<$<CONFIG:Release>:SS_RELEASE>
    $<$<CONFIG:Dist>:SS_DIST>
)

set_target_properties(Sandbox PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-x64/Sandbox"
)