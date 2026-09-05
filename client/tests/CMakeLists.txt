# Client GoogleTest Unit & Integration Test Suite
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# For Windows MSVC: Ensure shared CRT compatibility
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES *.cpp *.hpp)

add_executable(smart-ssh-tests
    ${TEST_SOURCES}
)

target_include_directories(smart-ssh-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(smart-ssh-tests PRIVATE
    gtest_main
    smart_ssh_installer
    smart_ssh_modules
    smart_ssh_core
    smart_ssh_ui
    smart_ssh_utils
)

set_target_properties(smart-ssh-tests PROPERTIES CXX_STANDARD 20)

include(GoogleTest)
gtest_discover_tests(smart-ssh-tests)
