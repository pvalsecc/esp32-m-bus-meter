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

add_executable(hdlc_frame_test hdlc_frame_test.cpp ../main/hdlc_frame.c ../main/buffer.c ../main/stream.c)
target_link_libraries(hdlc_frame_test GTest::gtest_main)
target_include_directories(hdlc_frame_test PRIVATE ../main .)
gtest_discover_tests(hdlc_frame_test)

add_executable(hdlc_fields_test hdlc_fields_test.cpp ../main/hdlc_fields.c ../main/buffer.c ../main/stream.c)
target_link_libraries(hdlc_fields_test GTest::gtest_main)
target_include_directories(hdlc_fields_test PRIVATE ../main .)
gtest_discover_tests(hdlc_fields_test)

add_executable(hdlc_packet_test hdlc_packet_test.cpp ../main/hdlc_packet.c ../main/hdlc_fields.c ../main/buffer.c ../main/stream.c)
target_link_libraries(hdlc_packet_test GTest::gtest_main)
target_include_directories(hdlc_packet_test PRIVATE ../main .)
gtest_discover_tests(hdlc_packet_test)

add_executable(dlms_test dlms_test.cpp ../main/dlms.c ../main/buffer.c ../main/stream.c)
target_link_libraries(dlms_test GTest::gtest_main)
target_include_directories(dlms_test PRIVATE ../main .)
gtest_discover_tests(dlms_test)

add_executable(pdu_test pdu_test.cpp ../main/pdu.c ../main/buffer.c ../main/stream.c)
target_link_libraries(pdu_test GTest::gtest_main)
target_include_directories(pdu_test PRIVATE ../main .)
gtest_discover_tests(pdu_test)

add_executable(buffer_test buffer_test.cpp ../main/buffer.c)
target_link_libraries(buffer_test GTest::gtest_main)
target_include_directories(buffer_test PRIVATE ../main .)
gtest_discover_tests(buffer_test)

add_executable(stream_test stream_test.cpp ../main/stream.c ../main/buffer.c)
target_link_libraries(stream_test GTest::gtest_main)
target_include_directories(stream_test PRIVATE ../main .)
gtest_discover_tests(stream_test)
