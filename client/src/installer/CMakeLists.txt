file(GLOB_RECURSE INSTALLER_SOURCES *.cpp *.hpp)

add_library(smart_ssh_installer STATIC
    ${INSTALLER_SOURCES}
)

target_include_directories(smart_ssh_installer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(smart_ssh_installer PRIVATE
    smart_ssh_ui
    smart_ssh_utils
)

set_target_properties(smart_ssh_installer PROPERTIES CXX_STANDARD 20)
