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

include(GoogleTest)

add_executable(hdlc_frame_test hdlc_frame_test.cpp ../main/hdlc_frame.c)
target_link_libraries(hdlc_frame_test GTest::gtest_main)
target_include_directories(hdlc_frame_test PRIVATE ../main .)
gtest_discover_tests(hdlc_frame_test)

add_executable(hdlc_fields_test hdlc_fields_test.cpp ../main/hdlc_fields.c)
target_link_libraries(hdlc_fields_test GTest::gtest_main)
target_include_directories(hdlc_fields_test PRIVATE ../main .)
gtest_discover_tests(hdlc_fields_test)

add_executable(hdlc_packet_test hdlc_packet_test.cpp ../main/hdlc_packet.c ../main/hdlc_fields.c)
target_link_libraries(hdlc_packet_test GTest::gtest_main)
target_include_directories(hdlc_packet_test PRIVATE ../main .)
gtest_discover_tests(hdlc_packet_test)
