cmake_minimum_required(VERSION 4.0)

project(frt
    VERSION 0.0.0
    DESCRIPTION "Result types for modern C++"
    LANGUAGES C CXX)

add_library(frt INTERFACE)
add_library(frt::frt ALIAS frt)

target_include_directories(frt INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(FRT_DEV_MODE "Enable strict checks" OFF)
if (FRT_DEV_MODE)
    target_compile_options(frt INTERFACE
        # GCC and Clang
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        >

        # MSVC
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /WX
        >
    )
endif ()

target_compile_options(frt INTERFACE
    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
    /Zc:preprocessor # For __VA_OPT__
    >
)

option(FRT_DEPENDENCIES_FROM_CPM "Use CPM for obtaining 3rd-party dependencies" OFF)
if (FRT_DEPENDENCIES_FROM_CPM)
    if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake")
        message(STATUS "Downloading CPM.cmake...")
        file(DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.1/CPM.cmake
            "${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake"
            EXPECTED_HASH SHA256=f3a6dcc6a04ce9e7f51a127307fa4f699fb2bade357a8eb4c5b45df76e1dc6a5
        )
    endif ()
    include("${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake")

    CPMAddPackage("gh:catchorg/Catch2@3.15.2")
    # For catch_discover_tests
    list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
endif ()

option(FRT_ENABLE_TESTING "Enable testing" OFF)
if (FRT_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()

OPTION(FRT_ENABLE_BENCHMARKING "Enable benchmarking" OFF)
if (FRT_ENABLE_BENCHMARKING)
    enable_testing()
    add_subdirectory(benchmark)
endif ()

OPTION(FRT_HARDENED "Contract violations terminate a program" ON)
if (FRT_HARDENED)
    target_compile_definitions(frt INTERFACE FRT_HARDENED=1)
endif ()

OPTION(FRT_OPTIMIZE_EXECUTION_PATH "Use [[likely]] and [[unlikely]] attributes to allow compiler to optimize" ON)
if (FRT_OPTIMIZE_EXECUTION_PATH)
    target_compile_definitions(frt INTERFACE FRT_OPTIMIZE_EXECUTION_PATH=1)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/frt-config-version.cmake
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

install(
    TARGETS frt
    EXPORT frt-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    EXPORT frt-targets
    FILE frt-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/frt-config-version.cmake
    ${frt_SOURCE_DIR}/cmake/frt-config.cmake
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake
)
install(
    DIRECTORY include/frt
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*"
)
export(EXPORT frt-targets FILE frt-config.cmake)