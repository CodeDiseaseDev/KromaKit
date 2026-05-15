cmake_minimum_required(VERSION 3.20)
project(KromaKit LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(KROMAKIT_BUILD_EXAMPLES "Build examples" ON)

file(GLOB_RECURSE KROMAKIT_SHARED_SRC CONFIGURE_DEPENDS
        src/*.cpp
)

# First pass: Linux only. Exclude platform folders that are not Linux/desktop.
list(FILTER KROMAKIT_SHARED_SRC EXCLUDE REGEX ".*/platform/ios/.*")
list(FILTER KROMAKIT_SHARED_SRC EXCLUDE REGEX ".*/platform/macos/.*")
list(FILTER KROMAKIT_SHARED_SRC EXCLUDE REGEX ".*/platform/windows/.*")
list(FILTER KROMAKIT_SHARED_SRC EXCLUDE REGEX ".*/platform/apple/.*")

add_library(kromakit STATIC
        ${KROMAKIT_SHARED_SRC}
        nanovg/nanovg.cpp
)

target_include_directories(kromakit
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/nanovg
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nanosvg/src
)

target_compile_options(kromakit PRIVATE
        -Wall
        -Wextra
        -MMD
        -MP
)

target_link_libraries(kromakit
        PUBLIC
        glfw
        GL
        dl
        pthread
        m
        asound
)

if(KROMAKIT_BUILD_EXAMPLES)
    add_executable(kromakit_demo
            main_linux.cpp
    )

    target_link_libraries(kromakit_demo PRIVATE kromakit)
endif()