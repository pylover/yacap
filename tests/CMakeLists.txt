# Testing
include(CTest)

add_library(helpers OBJECT helpers.c helpers.h)


add_custom_target(profile 
    DEPENDS 
    profile_test_help
    profile_test_carg
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


# Test carg
add_executable(test_carg 
    $<TARGET_OBJECTS:helpers>
    test_carg.c)
target_include_directories(test_carg PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_carg PUBLIC clog carg)
add_test(NAME test_carg COMMAND test_carg)
add_custom_target(profile_test_carg
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_carg>
)
