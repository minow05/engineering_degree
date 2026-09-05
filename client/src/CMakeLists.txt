# Include component subdirectories
add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(installer)
add_subdirectory(modules)

# Target 1: Primary Installer Tool (smart-ssh-installer.exe)
add_executable(smart-ssh-installer
    main_installer.cpp
)

target_include_directories(smart-ssh-installer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(smart-ssh-installer PRIVATE
    smart_ssh_installer
    smart_ssh_core
    smart_ssh_ui
    smart_ssh_utils
)

# Target 2: Standalone Host-Bound App (smart-ssh-minowRPI.exe)
add_executable(smart-ssh-minowRPI
    main_host_app.cpp
)

target_include_directories(smart-ssh-minowRPI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(smart-ssh-minowRPI PRIVATE
    smart_ssh_modules
    smart_ssh_core
    smart_ssh_ui
    smart_ssh_utils
)

set_target_properties(smart-ssh-installer PROPERTIES CXX_STANDARD 20)
set_target_properties(smart-ssh-minowRPI PROPERTIES CXX_STANDARD 20)
