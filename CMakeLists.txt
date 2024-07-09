cmake_minimum_required(VERSION 3.7)
project(carg 
    VERSION 1.0.0
    LANGUAGES C
)


# GCC and it's flags
set(CMAKE_C_COMPILER gcc)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fms-extensions")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


# Build type and debug symbols
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Include debug symbols" FORCE)
endif()
if(CMAKE_BUILD_TYPE!=Release)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()


set(CARG_MAXOPTIONS 256 CACHE STRING "Maximum allowed options")
set_property(CACHE CARG_MAXOPTIONS PROPERTY STRINGS 64 128 256 512 1024)

set(CARG_TEMPBUFFSIZE 256 CACHE STRING "Option temp buffer size")
set_property(CACHE CARG_TEMPBUFFSIZE PROPERTY STRINGS 64 128 256 512 1024)


configure_file(config.h.in config.h)
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)


add_library(option OBJECT option.c option.h)
add_library(optiondb OBJECT optiondb.c optiondb.h)
add_library(tokenizer OBJECT tokenizer.c tokenizer.h)
add_library(carg STATIC 
    carg.c carg.h
    $<TARGET_OBJECTS:option>
    $<TARGET_OBJECTS:optiondb>
    $<TARGET_OBJECTS:tokenizer>
)
target_link_libraries(carg PUBLIC clog)


# Install
install(TARGETS carg DESTINATION "lib")
install(FILES carg.h DESTINATION "include")


# Menu config
add_custom_target(menu COMMAND ccmake ${PROJECT_BINARY_DIR} )
add_custom_target(fresh COMMAND cmake ${PROJECT_SOURCE_DIR} --fresh)


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
    enable_testing()
    # list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")
    add_subdirectory(tests)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()


# # Examples
# if (NOT DEFINED ENV{SKIP_EXAMPLES})
# add_subdirectory(examples)
# endif()
