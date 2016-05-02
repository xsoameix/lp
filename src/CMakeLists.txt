# flags
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_C_COMPILER_ID}" MATCHES  "Clang")
  set(FLAGS
    "-std=c99 -pthread -pedantic -g"
    "-Wall -Wextra -Werror -Wconversion -Wshadow -Wwrite-strings")
endif()
string(REPLACE ";" " " SOURCE_FLAGS "${FLAGS}")
string(REPLACE " " ";" TARGET_FLAGS "${FLAGS}")

# main - main program
add_executable(main main.c scan.c)
target_compile_options(main PRIVATE ${TARGET_FLAGS})
