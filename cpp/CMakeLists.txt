cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_CXX_STANDARD 20)

set(SYSML_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SYSML_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SYSML_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SYSML_THIRDPARTY_DIR ${SYSML_BINARY_DIR}/extern)

option(SYSML_BUILD_TESTS "Build SYSML tests" ON)

option(SYSML_INCLUDE_CODE_GENERATOR "Include codegen (needs xbyak)" ON)

project(sysmlcpp
  LANGUAGES CXX
  VERSION 0.1.0)

set(SYSML_COMMON_SRC_CPP_FILES
  src/sysml.cpp)

add_library(sysmlcpp
  ${SYSML_COMMON_SRC_CPP_FILES})

target_include_directories(${PROJECT_NAME}
  PUBLIC ${PROJECT_BINARY_DIR})

target_include_directories(${PROJECT_NAME}
  PUBLIC include)


option(SYSML_DEBUG "Set to ON to build debug version" OFF)
option(SYSML_DEBUG_WERROR "Set to ON to enable all warnings in debug mode" ON)

if (SYSML_DEBUG)
  message(STATUS "Will compile libsysml in debug mode.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  if (SYSML_DEBUG_WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-sign-compare")
  endif()
else()
  message(STATUS "Will compile libsysml in release mode.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -O3 -Wall -Wextra -Werror -Wno-sign-compare")
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  if (SYSML_BUILD_TESTS)
   add_subdirectory(tests)
  endif()
endif()


if (SYSML_INCLUDE_CODE_GENERATOR)

  include(FetchContent)

  FetchContent_Declare(
    XByak
    GIT_REPOSITORY https://github.com/herumi/xbyak
    GIT_TAG        v6.60
    SOURCE_DIR xbyak
    )

  FetchContent_Declare(
    XByak_aarch64
    GIT_REPOSITORY https://github.com/zlateski/xbyak_aarch64
    GIT_TAG origin/fjmaster
    SOURCE_DIR xbyak_aarch64
    )

  FetchContent_MakeAvailable(XByak)
  target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_BINARY_DIR}/xbyak)
  # set(libsysml_INCLUDE_DIRS "${libsysml_INCLUDE_DIRS} ${PROJECT_BINARY_DIR}/xbyak")

  FetchContent_MakeAvailable(XByak_aarch64)
  target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_BINARY_DIR}/xbyak_aarch64)
  # set(libsysml_INCLUDE_DIRS "${libsysml_INCLUDE_DIRS} ${PROJECT_BINARY_DIR}/xbyak_aarch64")

  if(NOT (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR))
    set(libsysml_INCLUDE_DIRS ${SYSML_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/xbyak ${PROJECT_BINARY_DIR}/xbyak_aarch64 PARENT_SCOPE)
  endif()

  message(STATUS ${PROJECT_BINARY_DIR})
  message(STATUS ${PROJECT_BINARY_DIR})

  message(STATUS ${XByak_SOURCE_DIR})
  message(STATUS ${XByak_BINARY_DIR})

else()
  if(NOT (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR))
    set(libsysml_INCLUDE_DIRS ${SYSML_SOURCE_DIR}/include PARENT_SCOPE)
  endif()
endif()
