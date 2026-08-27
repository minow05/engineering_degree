# Client CLI Application CMake Entry
project(SmartSSHClient VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
