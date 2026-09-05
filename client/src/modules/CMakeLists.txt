file(GLOB_RECURSE MODULES_SOURCES *.cpp *.hpp)

add_library(smart_ssh_modules STATIC
    ${MODULES_SOURCES}
)

target_include_directories(smart_ssh_modules PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(smart_ssh_modules PRIVATE
    smart_ssh_core
    smart_ssh_ui
    smart_ssh_utils
)

set_target_properties(smart_ssh_modules PROPERTIES CXX_STANDARD 20)
