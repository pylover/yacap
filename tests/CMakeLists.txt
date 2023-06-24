# Testing
include(CTest)

list(APPEND testrules
    help
    logverbosity
    version
    options
    positional
)
list(TRANSFORM testrules PREPEND test_)
set(profilerules ${testrules})
list(TRANSFORM profilerules PREPEND profile_)
add_custom_target(profile DEPENDS ${profilerules})


add_library(helpers OBJECT helpers.c helpers.h)


foreach (t IN LISTS testrules) 
    # Test help
    add_executable(${t} 
        $<TARGET_OBJECTS:helpers>
        ${t}.c)
    target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")
    target_link_libraries(${t} PUBLIC clog carg)
    add_test(NAME ${t} COMMAND ${t})
    add_custom_target(run_${t} $<TARGET_FILE:${t}>)
    add_custom_target(profile_${t}
        COMMAND "valgrind" 
        ${VALGRIND_FLAGS}
        $<TARGET_FILE:${t}>
    )
endforeach()
