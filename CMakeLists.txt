cmake_minimum_required(VERSION 3.7)
project(carg 
    VERSION 1.0.0
    LANGUAGES C
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fms-extensions -D_GNU_SOURCE=1")


configure_file(manifest.h.in manifest.h)
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)


add_library(carg STATIC 
    carg.c carg.h
    print.c print.h
)


# Install
install(TARGETS carg DESTINATION "lib")
install(FILES carg.h DESTINATION "include")


# Lint
set(PRETTYC_FLAGS
    --recursive
    --verbose=0
    --repository=.
    --extensions=c,h,in
    --linelength=80
    --headers=h,in
    --includeorder=standardcfirst
    --root=.
    #--quiet
    ${PROJECT_SOURCE_DIR}
)
add_custom_target(lint
    COMMAND prettyc
    ${PRETTYC_FLAGS}
)


# CPack
set(CPACK_PACKAGE_FILE_NAME "libcarg-${PROJECT_VERSION}")
set(CPACK_SET_DESTDIR true)
set(CPACK_PACKAGE_NAME libcarg)
set(CPACK_PACKAGE_CONTACT "Vahid Mardani <vahid.mardani@gmail.com>")
set(CPACK_GENERATOR DEB)
include(CPack)


# Testing
if (NOT DEFINED ENV{SKIP_TESTS})
include(CTest)
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")


# Valgrind, Profiling
set(VALGRIND_FLAGS
    -s
    --tool=memcheck 
    --leak-check=yes 
    --show-reachable=yes 
    --num-callers=20 
    --track-fds=yes 
)

add_custom_target(profile 
    DEPENDS 
    profile_test_carg
)

# Test carg
add_executable(test_carg 
    $<TARGET_OBJECTS:carg>
    test_carg.c)
target_include_directories(test_carg PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(test_carg PUBLIC clog)
add_test(NAME test_carg COMMAND test_carg)
add_custom_target(profile_test_carg
    COMMAND "valgrind" 
    ${VALGRIND_FLAGS}
    $<TARGET_FILE:test_carg>
)
endif()


# # Examples
# if (NOT DEFINED ENV{SKIP_EXAMPLES})
# add_subdirectory(examples)
# endif()
