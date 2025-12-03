cmake_minimum_required(VERSION 3.15)
project(TseitinSpectralComplexity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(tseitin_lib
    src/graph.cpp
    src/tseitin_cnf.cpp
    src/kissat_runner.cpp
)

target_include_directories(tseitin_lib PUBLIC include)

add_executable(tseitin_app src/main.cpp)

target_link_libraries(tseitin_app PRIVATE tseitin_lib)

