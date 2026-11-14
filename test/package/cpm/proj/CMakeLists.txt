cmake_minimum_required(VERSION 4.4)
project(frt_cpm_tst)

set(CMAKE_CXX_STANDARD 26)

file(DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake"
    EXPECTED_HASH SHA256=f3a6dcc6a04ce9e7f51a127307fa4f699fb2bade357a8eb4c5b45df76e1dc6a5
)
include("${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake")
CPMAddPackage("gh:vt4a2h/frt@0.0.2")

add_executable(frt_cpm_tst main.cpp)

target_link_libraries(frt_cpm_tst PRIVATE frt::frt)