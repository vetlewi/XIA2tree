# This CMakeLists is meant to be added via add_subdirectory(test)
# from the top-level project when ENABLE_TESTS=ON.

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/doctest_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_cli.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_particle_range.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_tasks_parallel_end.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_tasks_splitter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_tasks_triggers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_config_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/integration_run_sort.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/integration_merge_files.cpp
)

if(NOT COMMAND CPMAddPackage)
    include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CPM.cmake)
endif()

CPMAddPackage("gh:doctest/doctest@2.4.12")

add_executable(XIA2tree_tests ${TEST_SOURCES})
set_target_properties(XIA2tree_tests PROPERTIES CXX_STANDARD 17)

target_link_libraries(XIA2tree_tests doctest::doctest XIAfuncs)

if(ENABLE_SANITIZERS)
    target_compile_options(XIA2tree_tests PRIVATE ${SANITIZER_FLAGS})
    target_link_options(XIA2tree_tests PRIVATE ${SANITIZER_FLAGS})
endif()

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(XIA2tree_tests)
