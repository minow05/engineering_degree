# Client Unit Tests Configuration
file(GLOB_RECURSE TEST_SOURCES *.cpp *.hpp)

if(TEST_SOURCES)
    add_executable(smart-ssh-tests
        ${TEST_SOURCES}
    )

    target_include_directories(smart-ssh-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )

    target_link_libraries(smart-ssh-tests PRIVATE
        smart_ssh_core
        smart_ssh_utils
    )

    add_test(NAME SmartSSHUnitTest COMMAND smart-ssh-tests)
endif()
