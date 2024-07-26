list(APPEND examples
  minimal
  iproute2
)


foreach (t IN LISTS examples) 
  # Test help
  add_executable(${t} 
    ${t}.c
  )
  target_link_libraries(${t} PRIVATE yacap)
  target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")
  add_custom_target(${t}_exec COMMAND ${t})
  add_custom_target(${t}_debug COMMAND gdb 
    --command=${PROJECT_SOURCE_DIR}/examples/${t}.gdb
    ./${t}
  )

  add_custom_target(${t}_profile
    COMMAND "valgrind" ${VALGRIND_FLAGS} ./${t}
  )
endforeach()
