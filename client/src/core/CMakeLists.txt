# Target library for core logic (SSH config, keys, vault, connections, system ops)
file(GLOB_RECURSE CORE_SOURCES *.cpp *.hpp)

add_library(smart_ssh_core STATIC
    ${CORE_SOURCES}
)

target_include_directories(smart_ssh_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(smart_ssh_core PRIVATE
    smart_ssh_utils
)

set_target_properties(smart_ssh_core PROPERTIES CXX_STANDARD 20)
