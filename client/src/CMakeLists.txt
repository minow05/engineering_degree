# Include component subdirectories
add_subdirectory(utils)
add_subdirectory(ui)
add_subdirectory(core)
add_subdirectory(installer)
add_subdirectory(modules)

# Target: Single SmartSSH Client Executable (smart-ssh)
add_executable(smart-ssh
    main.cpp
)

target_include_directories(smart-ssh PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(smart-ssh PRIVATE
    smart_ssh_ui
    smart_ssh_installer
    smart_ssh_modules
    smart_ssh_core
    smart_ssh_utils
)

set_target_properties(smart-ssh PROPERTIES CXX_STANDARD 20)
