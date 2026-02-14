cmake_minimum_required(VERSION 3.28)

project(frt
    VERSION 0.0.0
    DESCRIPTION "Result types for modern C++"
    LANGUAGES C CXX)

add_library(frt INTERFACE)
add_library(frt::frt ALIAS frt)

target_compile_features(frt INTERFACE cxx_std_23)

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

    set_target_properties(frt PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endif ()

option(FRT_ENABLE_TESTING "Enable testing" OFF)
if (FRT_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()