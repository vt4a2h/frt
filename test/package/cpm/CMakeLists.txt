add_test(
    NAME
    configure_tst_proj_cpm
    COMMAND
    ${CMAKE_COMMAND}
    -S ${CMAKE_CURRENT_SOURCE_DIR}/proj
    -B ${CMAKE_CURRENT_BINARY_DIR}
    -G ${CMAKE_GENERATOR}
)
add_test(
    NAME
    build_tst_proj_cpm
    COMMAND
    ${CMAKE_COMMAND}
    --build ${CMAKE_CURRENT_BINARY_DIR}
    --target all
)
add_test(
    NAME
    run_tst_proj_cpm
    COMMAND
    frt_cpm_tst
)

set_tests_properties(configure_tst_proj_cpm PROPERTIES FIXTURES_SETUP config_fixture)
set_tests_properties(build_tst_proj_cpm PROPERTIES FIXTURES_REQUIRED config_fixture)

set_tests_properties(build_tst_proj_cpm PROPERTIES FIXTURES_SETUP build_fixture)
set_tests_properties(run_tst_proj_cpm PROPERTIES FIXTURES_REQUIRED build_fixture)