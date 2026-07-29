cmake_minimum_required(VERSION 3.20)
project(MetaheuristicsOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Globalna ścieżka wyjściowa dla skompilowanych plików (.exe, .dll)
# ------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")


# ------------------------------------------------------------------------------
# Zewnętrzne zależności (Google Benchmark)
# ------------------------------------------------------------------------------
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/benchmark EXCLUDE_FROM_ALL SYSTEM)

# ------------------------------------------------------------------------------
# Flagi kompilatora dla wydajności
# ------------------------------------------------------------------------------
add_library(mh_flags INTERFACE)
if (MSVC)
    target_compile_options(mh_flags INTERFACE /O2 /Ob2 /fp:fast /arch:AVX2)
else()
    target_compile_options(mh_flags INTERFACE -O3 -ffast-math -march=native)
endif()

# ------------------------------------------------------------------------------
# 1. Główna biblioteka metaheurystyk (Header-only)
# ------------------------------------------------------------------------------
add_library(mh INTERFACE)
target_include_directories(mh INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
target_link_libraries(mh INTERFACE mh_flags)

# ------------------------------------------------------------------------------
# 2. Projekt: Eksperymenty (Executable)
# ------------------------------------------------------------------------------
file(GLOB_RECURSE EXPERIMENT_SOURCES CONFIGURE_DEPENDS
    "src/experiments/*.cpp" "src/experiments/*.h" "src/experiments/*.hpp"
)

add_executable(experiments ${EXPERIMENT_SOURCES})
target_include_directories(experiments PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/experiments")
target_link_libraries(experiments PRIVATE mh)
set_target_properties(experiments PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# ------------------------------------------------------------------------------
# 3. Projekt: Benchmarks (Executable)
# ------------------------------------------------------------------------------
file(GLOB_RECURSE BENCHMARK_SOURCES CONFIGURE_DEPENDS
    "src/benchmarks/*.cpp" "src/benchmarks/*.h" "src/benchmarks/*.hpp"
)

add_executable(benchmarks ${BENCHMARK_SOURCES})
target_link_libraries(benchmarks PRIVATE mh benchmark::benchmark)