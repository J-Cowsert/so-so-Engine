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

include(CTest)
option(SS_BUILD_ENGINE "Build the OpenGL engine and Sandbox application" ON)
option(SS_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer on Linux" OFF)

if(SS_ENABLE_SANITIZERS)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "SS_ENABLE_SANITIZERS requires GCC or Clang on Linux")
    endif()
endif()

# Context-free production code, shared by the renderer and unit tests.
add_library(soso-core STATIC
    so-so/src/so-so/Core/Layer.cpp
    so-so/src/so-so/Core/UID.cpp
)
target_include_directories(soso-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/so-so/src")
target_compile_options(soso-core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic>
)
set_target_properties(soso-core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-x64/so-so"
    COMPILE_WARNING_AS_ERROR ON
)

if(SS_ENABLE_SANITIZERS)
    target_compile_options(soso-core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
    target_link_options(soso-core PUBLIC -fsanitize=address,undefined)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(SS_BUILD_ENGINE)
    include(cmake/Engine.cmake)
endif()
