# Testing
include(CTest)
list(APPEND testrules
	tokenizer
    option
    # command
    # help
    # verbose
    # version
    # positional
)
list(TRANSFORM testrules PREPEND test_)
set(profilerules ${testrules})
list(TRANSFORM profilerules PREPEND profile_)
add_custom_target(profile DEPENDS ${profilerules})
add_library(helpers OBJECT helpers.c helpers.h)


# Valgrind, Profiling
set(VALGRIND_FLAGS
    -s
    --tool=memcheck 
    --leak-check=yes 
    --show-reachable=yes 
    --num-callers=20 
    --track-fds=yes 
)


foreach (t IN LISTS testrules) 
    # Test help
    add_executable(${t} ${t}.c $<TARGET_OBJECTS:helpers>)
    target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")
    target_link_libraries(${t} PUBLIC clog carg)
    add_test(NAME ${t} COMMAND ${t})
    add_custom_target(${t}_exec 
        COMMAND ctest --verbose -R ${t}
    )
    add_custom_target(${t}_debug COMMAND gdb 
      --command=${PROJECT_SOURCE_DIR}/tests/${t}.gdb
      ./${t}
    )
    add_custom_target(profile_${t}
        COMMAND "valgrind" ${VALGRIND_FLAGS} ctest -R ${t}
    )
endforeach()
