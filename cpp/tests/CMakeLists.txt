set(SYSML_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)

message(STATUS "Building tests.")

# Pull in the Catch2 framework.
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.x)

FetchContent_MakeAvailable(Catch2)

add_library(sysml_tests_catch2_main
  catch2_main.cpp)

target_link_libraries(sysml_tests_catch2_main
  PUBLIC Catch2)

function(sysml_test name)
  message(STATUS "sysml_test ${name}_test ${name}.cpp")
  add_executable(${name}_test ${name}.cpp)
  target_link_libraries(${name}_test
    PUBLIC sysmlcpp
    PUBLIC sysml_tests_catch2_main
    PUBLIC -lpthread
    PUBLIC Catch2::Catch2)
endfunction(sysml_test)

sysml_test(vek)
sysml_test(numeric)
sysml_test(observed_ptr)
