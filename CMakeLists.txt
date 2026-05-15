cmake_minimum_required(VERSION 3.25)

project(KromaKit
        VERSION 0.1.0
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(KROMAKIT_BUILD_EXAMPLES "Build KromaKit examples/demo apps" ON)
option(KROMAKIT_BUILD_TESTS "Build KromaKit tests" OFF)

# First pass: Linux only.
set(KROMAKIT_PLATFORM_LINUX OFF)

set(KROMAKIT_STATIC_LIBRARY "${KROMAKIT_BUILD_DIR}/lib/libkromakit.a" CACHE FILEPATH "Path to libkromakit.a")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(KROMAKIT_PLATFORM_LINUX ON)
else()
    message(FATAL_ERROR "KromaKit CMake currently supports Linux only")
endif()

file(GLOB_RECURSE KROMAKIT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Exclude non-Linux platform implementations for now.
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/ios/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/macos/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/windows/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/apple/.*")

add_library(kromakit STATIC
        ${KROMAKIT_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/nanovg/nanovg.cpp"
)

add_library(KromaKit::kromakit ALIAS kromakit)

target_compile_features(kromakit PUBLIC cxx_std_23)

target_include_directories(kromakit
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/nanovg"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/nanosvg/src"
)

target_compile_definitions(kromakit
        PUBLIC
        KROMAKIT_PLATFORM_LINUX=1
)

target_compile_options(kromakit
        PRIVATE
        -Wall
        -Wextra
)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(GLFW3 REQUIRED glfw3)
pkg_check_modules(ALSA REQUIRED alsa)

target_include_directories(kromakit
        PUBLIC
        ${GLFW3_INCLUDE_DIRS}
        ${ALSA_INCLUDE_DIRS}
)

target_link_directories(kromakit
        PUBLIC
        ${GLFW3_LIBRARY_DIRS}
        ${ALSA_LIBRARY_DIRS}
)

target_link_libraries(kromakit
        PUBLIC
        ${GLFW3_LIBRARIES}
        ${ALSA_LIBRARIES}
        OpenGL::GL
        Threads::Threads
        dl
        m
)

if(GLFW3_CFLAGS_OTHER)
    target_compile_options(kromakit PUBLIC ${GLFW3_CFLAGS_OTHER})
endif()

if(ALSA_CFLAGS_OTHER)
    target_compile_options(kromakit PUBLIC ${ALSA_CFLAGS_OTHER})
endif()

set_target_properties(kromakit PROPERTIES
        OUTPUT_NAME "kromakit"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(KROMAKIT_BUILD_EXAMPLES)
    add_executable(kromakit_demo
            "${CMAKE_CURRENT_SOURCE_DIR}/main_linux.cpp"
    )

    target_link_libraries(kromakit_demo
            PRIVATE
            KromaKit::kromakit
    )

    set_target_properties(kromakit_demo PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

