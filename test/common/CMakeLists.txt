add_executable(tests_common)

target_sources(tests_common PRIVATE
    tst_common.cpp
)

target_compile_features(tests_common PRIVATE cxx_std_23)

set_target_properties(tests_common PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(tests_common PRIVATE
    frt::frt

    Catch2::Catch2
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(tests_common)