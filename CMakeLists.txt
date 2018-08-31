project(capnp-test)
cmake_minimum_required(VERSION 3.8.0)

# set(CMAKE_COMMON_FLAGS "-dead_strip")

cmake_policy(SET CMP0069 NEW)
include(CheckIPOSupported)
check_ipo_supported()

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Werror")

#set(CapnProto_DIR /Users/proffan/Projects/Development/Robotics/capnp-test/local/lib/cmake/CapnProto)

find_package(CapnProto REQUIRED)

capnp_generate_cpp(pubsubSources pubsubHeaders schema/pubsub.capnp)
#capnp_generate_cpp(msgSources msgHeaders test.capnp)

add_executable(test_capnp src/main.cpp ${pubsubSources})

target_link_libraries(test_capnp PRIVATE CapnProto::capnp-rpc -dead_strip)
target_include_directories(test_capnp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_property(TARGET test_capnp PROPERTY INTERPROCEDURAL_OPTIMIZATION True)