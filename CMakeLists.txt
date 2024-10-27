# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
if (DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(m-bus-meter)
else()
    project(m-bus-meter LANGUAGES C CXX)
    message(STATUS "Enable tests")
    enable_testing()
    add_subdirectory(unittests)
endif ()
