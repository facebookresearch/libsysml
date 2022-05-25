cmake_minimum_required(VERSION 3.15.0)

project(sysmlcpp
  LANGUAGES CXX
  VERSION 0.0.0)

set(header_files
  ${CMAKE_CURRENT_SOURCE_DIR}/include/sysml/measure.hpp)

add_library(sysmlcpp INTERFACE)

target_sources(sysmlcpp
  INTERFACE
  "$<BUILD_INTERFACE:${header_files}>")

target_include_directories(sysmlcpp
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

target_include_directories(sysmlcpp
  SYSTEM INTERFACE
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
