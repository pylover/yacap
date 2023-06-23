# Testing
include(CTest)

add_library(helpers OBJECT helpers.c helpers.h)


add_custom_target(profile 
    DEPENDS 
    profile_test_help
    profile_test_version
    profile_test_logverbosity
    profile_test_options
    profile_test_positional
)


# Test help
add_executable(test_help 
    $<TARGET_OBJECTS:helpers>
    test_help.c)
target_include_directories(test_help PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_help PUBLIC clog carg)
add_test(NAME test_help COMMAND test_help)
add_custom_target(profile_test_help
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_help>
)


# Test version
add_executable(test_version 
    $<TARGET_OBJECTS:helpers>
    test_version.c)
target_include_directories(test_version PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_version PUBLIC clog carg)
add_test(NAME test_version COMMAND test_version)
add_custom_target(profile_test_version
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_version>
)


# Test logverbosity
add_executable(test_logverbosity 
    $<TARGET_OBJECTS:helpers>
    test_logverbosity.c)
target_include_directories(test_logverbosity PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_logverbosity PUBLIC clog carg)
add_test(NAME test_logverbosity COMMAND test_logverbosity)
add_custom_target(profile_test_logverbosity
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_logverbosity>
)


# Test options
add_executable(test_options 
    $<TARGET_OBJECTS:helpers>
    test_options.c)
target_include_directories(test_options PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_options PUBLIC clog carg)
add_test(NAME test_options COMMAND test_options)
add_custom_target(profile_test_options
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_options>
)


# Test positional
add_executable(test_positional 
    $<TARGET_OBJECTS:helpers>
    test_positional.c)
target_include_directories(test_positional PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_positional PUBLIC clog carg)
add_test(NAME test_positional COMMAND test_positional)
add_custom_target(profile_test_positional
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_positional>
)
