# Target library for UI & Terminal components
file(GLOB_RECURSE UI_SOURCES *.cpp *.hpp)

add_library(smart_ssh_ui STATIC
    ${UI_SOURCES}
)

target_include_directories(smart_ssh_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(smart_ssh_ui PRIVATE
    smart_ssh_utils
)

set_target_properties(smart_ssh_ui PROPERTIES CXX_STANDARD 20)
