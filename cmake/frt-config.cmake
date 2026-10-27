include("${CMAKE_CURRENT_LIST_DIR}/frt-targets.cmake")
add_library(frt::frt INTERFACE IMPORTED)
target_link_libraries(frt::frt INTERFACE frt)
