#cmake_minimum_required(VERSION 3.25)
#
#project(KromaKit
#        VERSION 0.1.0
#        LANGUAGES C CXX
#)
#
#set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#
#option(KROMAKIT_BUILD_EXAMPLES "Build KromaKit examples/demo apps" ON)
#option(KROMAKIT_BUILD_TESTS "Build KromaKit tests" OFF)
#
## First pass: Linux only.
#set(KROMAKIT_PLATFORM_LINUX OFF)
#
#set(KROMAKIT_STATIC_LIBRARY "${KROMAKIT_BUILD_DIR}/lib/libkromakit.a" CACHE FILEPATH "Path to libkromakit.a")
#
#if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    set(KROMAKIT_PLATFORM_LINUX ON)
#else()
#    message(FATAL_ERROR "KromaKit CMake currently supports Linux only")
#endif()
#
#file(GLOB_RECURSE KROMAKIT_SOURCES CONFIGURE_DEPENDS
#        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
#)
#
## Exclude non-Linux platform implementations for now.
#list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/ios/.*")
#list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/macos/.*")
#list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/windows/.*")
#list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/apple/.*")
#
#add_library(kromakit STATIC
#        ${KROMAKIT_SOURCES}
#        "${CMAKE_CURRENT_SOURCE_DIR}/nanovg/nanovg.cpp"
#)
#
#add_library(KromaKit::kromakit ALIAS kromakit)
#
#target_compile_features(kromakit PUBLIC cxx_std_23)
#
#target_include_directories(kromakit
#        PUBLIC
#        "${CMAKE_CURRENT_SOURCE_DIR}/include"
#        "${CMAKE_CURRENT_SOURCE_DIR}/nanovg"
#        PRIVATE
#        "${CMAKE_CURRENT_SOURCE_DIR}/src"
#        "${CMAKE_CURRENT_SOURCE_DIR}/external/nanosvg/src"
#)
#
#target_compile_definitions(kromakit
#        PUBLIC
#        KROMAKIT_PLATFORM_LINUX=1
#)
#
#target_compile_options(kromakit
#        PRIVATE
#        -Wall
#        -Wextra
#)
#
#find_package(PkgConfig REQUIRED)
#find_package(OpenGL REQUIRED)
#find_package(Threads REQUIRED)
#
#pkg_check_modules(GLFW3 REQUIRED glfw3)
#pkg_check_modules(ALSA REQUIRED alsa)
#
#target_include_directories(kromakit
#        PUBLIC
#        ${GLFW3_INCLUDE_DIRS}
#        ${ALSA_INCLUDE_DIRS}
#)
#
#target_link_directories(kromakit
#        PUBLIC
#        ${GLFW3_LIBRARY_DIRS}
#        ${ALSA_LIBRARY_DIRS}
#)
#
#target_link_libraries(kromakit
#        PUBLIC
#        ${GLFW3_LIBRARIES}
#        ${ALSA_LIBRARIES}
#        OpenGL::GL
#        Threads::Threads
#        dl
#        m
#)
#
#if(GLFW3_CFLAGS_OTHER)
#    target_compile_options(kromakit PUBLIC ${GLFW3_CFLAGS_OTHER})
#endif()
#
#if(ALSA_CFLAGS_OTHER)
#    target_compile_options(kromakit PUBLIC ${ALSA_CFLAGS_OTHER})
#endif()
#
#set_target_properties(kromakit PROPERTIES
#        OUTPUT_NAME "kromakit"
#        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#)
#
#if(KROMAKIT_BUILD_EXAMPLES)
#    add_executable(kromakit_demo
#            "${CMAKE_CURRENT_SOURCE_DIR}/main_linux.cpp"
#    )
#
#    target_link_libraries(kromakit_demo
#            PRIVATE
#            KromaKit::kromakit
#    )
#
#    set_target_properties(kromakit_demo PROPERTIES
#            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#    )
#endif()
#


cmake_minimum_required(VERSION 3.25)

project(KromaKit
        VERSION 0.1.0
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
endif()

option(KROMAKIT_BUILD_EXAMPLES "Build KromaKit examples/demo apps" ON)
option(KROMAKIT_BUILD_TESTS "Build KromaKit tests" OFF)

# ------------------------------------------------------------
# Platform detection
# ------------------------------------------------------------

set(KROMAKIT_PLATFORM_LINUX OFF)
set(KROMAKIT_PLATFORM_WINDOWS OFF)
set(KROMAKIT_PLATFORM_MACOS OFF)
set(KROMAKIT_PLATFORM_IOS OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(KROMAKIT_PLATFORM_IOS ON)
elseif(APPLE)
    set(KROMAKIT_PLATFORM_MACOS ON)
elseif(WIN32)
    set(KROMAKIT_PLATFORM_WINDOWS ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(KROMAKIT_PLATFORM_LINUX ON)
else()
    message(FATAL_ERROR "Unsupported KromaKit platform: ${CMAKE_SYSTEM_NAME}")
endif()

message(STATUS "KromaKit platform:")
message(STATUS "  Linux:   ${KROMAKIT_PLATFORM_LINUX}")
message(STATUS "  Windows: ${KROMAKIT_PLATFORM_WINDOWS}")
message(STATUS "  macOS:   ${KROMAKIT_PLATFORM_MACOS}")
message(STATUS "  iOS:     ${KROMAKIT_PLATFORM_IOS}")

# ------------------------------------------------------------
# Sources
# ------------------------------------------------------------

file(GLOB_RECURSE KROMAKIT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.m"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm"
)

# Remove all platform implementations first.
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/linux/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/windows/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/apple/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/ios/.*")
list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*/platform/macos/.*")

set(KROMAKIT_PLATFORM_SOURCES "")

if(KROMAKIT_PLATFORM_LINUX)
    file(GLOB_RECURSE KROMAKIT_PLATFORM_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/linux/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/linux/*.c"
    )
elseif(KROMAKIT_PLATFORM_WINDOWS)
    file(GLOB_RECURSE KROMAKIT_PLATFORM_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/windows/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/windows/*.c"
    )
elseif(KROMAKIT_PLATFORM_MACOS)
    file(GLOB_RECURSE KROMAKIT_PLATFORM_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/apple/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/apple/*.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/macos/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/macos/*.mm"
    )
elseif(KROMAKIT_PLATFORM_IOS)
    file(GLOB_RECURSE KROMAKIT_PLATFORM_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/apple/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/apple/*.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/ios/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/ios/*.mm"
    )
endif()

list(APPEND KROMAKIT_SOURCES ${KROMAKIT_PLATFORM_SOURCES})

if(NOT APPLE)
    list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*\\.m$")
    list(FILTER KROMAKIT_SOURCES EXCLUDE REGEX ".*\\.mm$")
endif()

# ------------------------------------------------------------
# Library
# ------------------------------------------------------------

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

set_target_properties(kromakit PROPERTIES
        OUTPUT_NAME "kromakit"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ------------------------------------------------------------
# Compile definitions
# ------------------------------------------------------------

if(KROMAKIT_PLATFORM_LINUX)
    target_compile_definitions(kromakit PUBLIC
            KROMAKIT_PLATFORM_LINUX=1
    )
elseif(KROMAKIT_PLATFORM_WINDOWS)
    target_compile_definitions(kromakit PUBLIC
            KROMAKIT_PLATFORM_WINDOWS=1
    )
    set_target_properties(kromakit PROPERTIES
            CXX_EXTENSIONS ON
    )
elseif(KROMAKIT_PLATFORM_MACOS)
    target_compile_definitions(kromakit PUBLIC
            KROMAKIT_PLATFORM_MACOS=1
            KROMAKIT_PLATFORM_APPLE=1
    )
elseif(KROMAKIT_PLATFORM_IOS)
    target_compile_definitions(kromakit PUBLIC
            KROMAKIT_PLATFORM_IOS=1
            KROMAKIT_PLATFORM_APPLE=1
    )
endif()

# ------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------

if(MSVC)
    target_compile_options(kromakit PRIVATE
            /W4
            /permissive-
    )
else()
    target_compile_options(kromakit PRIVATE
            -Wall
            -Wextra
    )
endif()

# ------------------------------------------------------------
# Linux deps
# ------------------------------------------------------------

if(KROMAKIT_PLATFORM_LINUX)
    find_package(PkgConfig REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    pkg_check_modules(GLFW3 REQUIRED glfw3)
    pkg_check_modules(FREETYPE2 REQUIRED freetype2)

    target_include_directories(kromakit PUBLIC
            ${GLFW3_INCLUDE_DIRS}
            ${FREETYPE2_INCLUDE_DIRS}
    )

    target_link_directories(kromakit PUBLIC
            ${GLFW3_LIBRARY_DIRS}
            ${FREETYPE2_LIBRARY_DIRS}
    )

    target_link_libraries(kromakit PUBLIC
            ${GLFW3_LIBRARIES}
            ${FREETYPE2_LIBRARIES}
            OpenGL::GL
            Threads::Threads
            dl
            m
    )

    if(GLFW3_CFLAGS_OTHER)
        target_compile_options(kromakit PUBLIC ${GLFW3_CFLAGS_OTHER})
    endif()

    if(FREETYPE2_CFLAGS_OTHER)
        target_compile_options(kromakit PUBLIC ${FREETYPE2_CFLAGS_OTHER})
    endif()
endif()

# ------------------------------------------------------------
# Windows deps
# ------------------------------------------------------------

# ------------------------------------------------------------
# Windows deps
# ------------------------------------------------------------

if(KROMAKIT_PLATFORM_WINDOWS)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    target_link_libraries(kromakit PUBLIC
            OpenGL::GL
            Threads::Threads

            gdi32
            user32
            shell32
            ole32
            uuid
            dwmapi
    )

    # Important:
    # Do not use host pkg-config here during MinGW cross builds.
    # It can inject Linux include paths like /usr/include into the Windows build.
    #
    # GLFW/Freetype for Windows should be added later via:
    # - a MinGW toolchain file
    # - imported library targets
    # - FetchContent
    # - vendored/prebuilt MinGW libraries
    # - or PKG_CONFIG_LIBDIR pointing only at MinGW .pc files
endif()

# ------------------------------------------------------------
# macOS deps
# ------------------------------------------------------------

if(KROMAKIT_PLATFORM_MACOS)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(METALKIT_FRAMEWORK MetalKit REQUIRED)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)

    target_link_libraries(kromakit PUBLIC
            ${APPKIT_FRAMEWORK}
            ${FOUNDATION_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
            ${METAL_FRAMEWORK}
            ${METALKIT_FRAMEWORK}
            ${COREGRAPHICS_FRAMEWORK}
    )
endif()

# ------------------------------------------------------------
# iOS deps
# ------------------------------------------------------------

if(KROMAKIT_PLATFORM_IOS)
    find_library(UIKIT_FRAMEWORK UIKit REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(METALKIT_FRAMEWORK MetalKit REQUIRED)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)

    target_link_libraries(kromakit PUBLIC
            ${UIKIT_FRAMEWORK}
            ${FOUNDATION_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
            ${METAL_FRAMEWORK}
            ${METALKIT_FRAMEWORK}
            ${COREGRAPHICS_FRAMEWORK}
    )
endif()

# ------------------------------------------------------------
# Examples
# ------------------------------------------------------------

if(KROMAKIT_BUILD_EXAMPLES)
    set(KROMAKIT_DEMO_ENTRY "")

    if(KROMAKIT_PLATFORM_LINUX AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main_linux.cpp")
        list(APPEND KROMAKIT_DEMO_ENTRY "${CMAKE_CURRENT_SOURCE_DIR}/main_linux.cpp")
    elseif(KROMAKIT_PLATFORM_WINDOWS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main_windows.cpp")
        list(APPEND KROMAKIT_DEMO_ENTRY "${CMAKE_CURRENT_SOURCE_DIR}/main_windows.cpp")
    elseif(KROMAKIT_PLATFORM_MACOS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main_macos.mm")
        list(APPEND KROMAKIT_DEMO_ENTRY "${CMAKE_CURRENT_SOURCE_DIR}/main_macos.mm")
    elseif(KROMAKIT_PLATFORM_IOS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.mm")
        list(APPEND KROMAKIT_DEMO_ENTRY "${CMAKE_CURRENT_SOURCE_DIR}/main.mm")
    endif()

    if(KROMAKIT_DEMO_ENTRY)
        if(KROMAKIT_PLATFORM_MACOS OR KROMAKIT_PLATFORM_IOS)
            add_executable(kromakit_demo MACOSX_BUNDLE
                    ${KROMAKIT_DEMO_ENTRY}
            )
        else()
            add_executable(kromakit_demo
                    ${KROMAKIT_DEMO_ENTRY}
            )
        endif()

        target_link_libraries(kromakit_demo PRIVATE
                KromaKit::kromakit
        )

        set_target_properties(kromakit_demo PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    else()
        message(WARNING "KROMAKIT_BUILD_EXAMPLES is ON, but no demo entrypoint exists for this platform")
    endif()
endif()