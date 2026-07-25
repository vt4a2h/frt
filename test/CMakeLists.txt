find_package(Catch2 3 CONFIG REQUIRED)

function(frt_add_test test_name)
    string(REPLACE "_" "-" test_target_name "${test_name}")
    set(test_target_name "test-${test_target_name}")

    add_executable(${test_target_name})
    target_sources(${test_target_name} PRIVATE
        tst_${test_name}.cpp
    )

    target_link_libraries(${test_target_name} PRIVATE
        frt::frt
        frt::frt-test-helpers

        Catch2::Catch2
        Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(${test_target_name})

    set(frt_test_target_name ${test_target_name} PARENT_SCOPE)
endfunction()

add_subdirectory(helpers)
add_subdirectory(either)
add_subdirectory(expects)
