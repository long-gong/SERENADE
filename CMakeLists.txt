cmake_minimum_required(VERSION 3.0)

project(SERENADE)

set(CMAKE_CXX_STANDARD 11)

option(USE_GTest "Use Google Test for testing" ON)
option(Test_Extreme_Large_Cases "Turn on extreme large case tests" ON)

set(saber_libraries "")
if (USE_GTest)
    enable_testing()
    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
            ${CMAKE_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
        include_directories("${gtest_SOURCE_DIR}/include")
    endif()

    list(APPEND saber_libraries gtest_main)

endif()

add_library(serenade STATIC serenade.hpp)
target_include_directories(serenade PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(serenade PROPERTIES LINKER_LANGUAGE CXX)
if (Test_Extreme_Large_Cases)
    target_compile_definitions(serenade PRIVATE TEST_EXTREME_LARGE_CASES=1)
    list(APPEND saber_libraries gmp)
endif()

add_library(serena_graph_based serena_graph_based.hpp)
target_include_directories(serena_graph_based PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(serena_graph_based PROPERTIES LINKER_LANGUAGE CXX)

add_library(csv_utils csv_utils.hpp)
target_include_directories(csv_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(csv_utils PROPERTIES LINKER_LANGUAGE CXX)

list(APPEND saber_libraries serenade armadillo serena_graph_based csv_utils fmt)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/py/iterations_def.csv DESTINATION ${PROJECT_BINARY_DIR}/data)
add_subdirectory(test)
