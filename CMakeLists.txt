cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if(PRIMARY)
  # define preprocessor macro with the same name
  add_compile_definitions(PRIMARY=1)
else()
  add_compile_definitions(PRIMARY=0)
endif()

project(funkbox-display)
