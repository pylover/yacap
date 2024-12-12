# Testing
include(CTest)
list(APPEND testrules
  arghint
  autocompletion_bash
	option
	option_multiple
	optiondb
	tokenizer
	version
	help
	command
	command_help
	command_optionorder
  positional
	dashdash
)
if (YACAP_USE_CLOG)
	list(APPEND testrules clog)
endif()


list(TRANSFORM testrules PREPEND test_)
set(profilerules ${testrules})
list(TRANSFORM profilerules APPEND _profile)
add_custom_target(profile_tests DEPENDS ${profilerules})
add_library(pipewrap OBJECT pipewrap.c pipewrap.h)


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
  add_executable(${t} ${t}.c $<TARGET_OBJECTS:pipewrap>)
  target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")
	target_link_libraries(${t} PRIVATE yacap)
	if (YACAP_USE_CLOG)
		target_link_libraries(${t} PUBLIC clog)
	endif()
  add_test(NAME ${t} COMMAND ${t})

  add_custom_target(${t}_exec 
    COMMAND ctest --verbose -R "^${t}$"
    DEPENDS ${t} ${CMAKE_PROJECT_NAME}
  )
  add_custom_target(${t}_debug 
	  COMMAND gdb --command=${PROJECT_SOURCE_DIR}/tests/${t}.gdb ./${t}
	  DEPENDS ${t} ${CMAKE_PROJECT_NAME}
  )
  add_custom_target(${t}_profile
	  COMMAND "valgrind" ${VALGRIND_FLAGS} ${PROJECT_BINARY_DIR}/tests/${t}
	  DEPENDS ${t} ${CMAKE_PROJECT_NAME}
  )
endforeach()
