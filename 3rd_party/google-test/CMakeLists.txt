# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  #message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  #message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

set(GTEST_LIBRARY gtest
    CACHE INTERNAL
    "Documentation for GTEST_LIBRARY"
)
set(GTEST_MAIN_LIBRARY gtest_main
    CACHE INTERNAL
    "Documentation for GTEST_MAIN_LIBRARY"
)
set(GTEST_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
    CACHE INTERNAL
    "Documentation for GTEST_INCLUDE_DIR"
)

# Add aliases for GTest and GMock libraries
add_library(GTest::GTest ALIAS gtest)
add_library(GTest::Main ALIAS gtest_main)
add_library(GMock::GMock ALIAS gmock)
add_library(GMock::Main ALIAS gmock_main)
