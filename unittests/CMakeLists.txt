cmake_minimum_required(VERSION 3.14)
project(m-bus-meter-uts)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(hdlc_frame_test hdlc_frame_test.cpp ../main/hdlc_frame.c)
target_link_libraries(hdlc_frame_test GTest::gtest_main)
target_include_directories(hdlc_frame_test PRIVATE ../main .)

include(GoogleTest)
gtest_discover_tests(hdlc_frame_test)
