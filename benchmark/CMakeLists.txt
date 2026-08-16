find_package(Catch2 3 CONFIG REQUIRED)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "The build type is Debug. Benchmarking results might be inaccurate.")
endif ()

function(frt_add_benchmark benchmark_name)
    string(REPLACE "_" "-" benchmark_target_name "${benchmark_name}")
    set(benchmark_target_name "test-${benchmark_target_name}")

    add_executable(${benchmark_target_name})
    target_sources(${benchmark_target_name} PRIVATE
        bench_${benchmark_name}.cpp
    )

    target_link_libraries(${benchmark_target_name} PRIVATE
        frt::frt

        Catch2::Catch2
        Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(${benchmark_target_name})

    set(frt_benchmark_target_name ${benchmark_target_name} PARENT_SCOPE)
endfunction()

add_subdirectory(either)