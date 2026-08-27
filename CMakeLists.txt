cmake_minimum_required(VERSION 3.16)
project(EngineeringDegreeProject VERSION 1.0.0 LANGUAGES CXX)

# Set global C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure strict compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build options
option(BUILD_CLIENT "Build Client C++ CLI Application" ON)
option(BUILD_TESTS "Build Unit Tests" ON)
option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis during build" OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
        message(STATUS "Clang-Tidy enabled: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "Clang-Tidy requested but executable not found.")
    endif()
endif()

# Add Client Application Subdirectory
if(BUILD_CLIENT)
    add_subdirectory(client)
endif()
