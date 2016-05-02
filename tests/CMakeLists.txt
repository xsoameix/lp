enable_testing()

# flags
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_C_COMPILER_ID}" MATCHES  "Clang")
  set(FLAGS "-Wall -Werror -std=c99 -pthread -pedantic")
endif()
string(REPLACE ";" " " SOURCE_FLAGS "${FLAGS}")
string(REPLACE " " ";" TARGET_FLAGS "${FLAGS}")

# Check
find_package(Check REQUIRED)
set(DIRS ${DIRS} ${CHECK_INCLUDE_DIR})
set(LIBS ${LIBS} ${CHECK_LIBRARIES})

# thread
find_package(Threads REQUIRED)
set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})

# suite - testing program
add_executable(suite
  ../src/scan.c
  scan.c)
target_include_directories(suite PRIVATE ${DIRS} ../src)
target_link_libraries(suite ${LIBS})
target_compile_options(suite PRIVATE ${TARGET_FLAGS})
add_test(suite ${CMAKE_CURRENT_BINARY_DIR}/suite)
