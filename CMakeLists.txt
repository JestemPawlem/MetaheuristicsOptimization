cmake_minimum_required(VERSION 3.20)
project(MetaheuristicsOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")


set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/benchmark EXCLUDE_FROM_ALL SYSTEM)


add_library(mh_flags INTERFACE)
if (MSVC)
    target_compile_options(mh_flags INTERFACE /O2 /Ob2 /fp:fast /arch:AVX2)
else()
    target_compile_options(mh_flags INTERFACE -O3 -ffast-math -march=native)
endif()


add_library(mh INTERFACE)
target_include_directories(mh INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
target_link_libraries(mh INTERFACE mh_flags)


file(GLOB_RECURSE EXPERIMENT_SOURCES CONFIGURE_DEPENDS
    "src/experiments/*.cpp" "src/experiments/*.h" "src/experiments/*.hpp"
)

add_executable(experiments ${EXPERIMENT_SOURCES} "src/experiments/de_experiments.h" "src/include/mh/algorithms/pso/inertia_policy.h")
target_include_directories(experiments PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/experiments")
target_link_libraries(experiments PRIVATE mh)
set_target_properties(experiments PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)


file(GLOB_RECURSE BENCHMARK_SOURCES CONFIGURE_DEPENDS
    "src/benchmarks/*.cpp" "src/benchmarks/*.h" "src/benchmarks/*.hpp"
)

add_executable(benchmarks ${BENCHMARK_SOURCES} "src/experiments/de_experiments.h" "src/include/mh/algorithms/pso/inertia_policy.h")
target_link_libraries(benchmarks PRIVATE mh benchmark::benchmark)