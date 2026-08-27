# Target library for utility helpers (Exec, process wrappers)
file(GLOB_RECURSE UTILS_SOURCES *.cpp *.hpp)

add_library(smart_ssh_utils STATIC
    ${UTILS_SOURCES}
)

target_include_directories(smart_ssh_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(smart_ssh_utils PROPERTIES CXX_STANDARD 20)
