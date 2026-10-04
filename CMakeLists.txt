cmake_minimum_required(VERSION 3.15)
project(onDemandBackend)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NGHTTP2 REQUIRED IMPORTED_TARGET libnghttp2)

add_executable(onDemandBackend main.cpp)
target_link_libraries(onDemandBackend PRIVATE PkgConfig::NGHTTP2)