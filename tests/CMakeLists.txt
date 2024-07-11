# Testing
include(CTest)
list(APPEND testrules
	option
	optiondb
	tokenizer
	version
    # command
    # help
    # verbose
    # positional
)
list(TRANSFORM testrules PREPEND test_)
set(profilerules ${testrules})
list(TRANSFORM profilerules APPEND _profile)
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


# link_libraries(

foreach (t IN LISTS testrules) 
    # Test help
    add_executable(${t} ${t}.c $<TARGET_OBJECTS:helpers>)
    target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")
	target_link_libraries(${t} PUBLIC clog PRIVATE carg)
    add_test(NAME ${t} COMMAND ${t})
    add_custom_target(${t}_exec 
        COMMAND ctest --verbose -R "^${t}$"
		DEPENDS ${t}
    )
    add_custom_target(${t}_debug 
		COMMAND gdb --command=${PROJECT_SOURCE_DIR}/tests/${t}.gdb ./${t}
		DEPENDS ${t}
    )
    add_custom_target(${t}_profile
        COMMAND "valgrind" ${VALGRIND_FLAGS} ctest -R ${t}
		DEPENDS ${t}
    )
endforeach()
