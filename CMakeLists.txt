cmake_minimum_required(VERSION 2.8.12)
project(benchmarks CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(benchmarks ${CMAKE_SOURCE_DIR}/src/main.cpp)
set_property(TARGET benchmarks PROPERTY CXX_STANDARD 11)

target_include_directories(benchmarks PRIVATE ${CONAN_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/)
target_link_libraries(benchmarks PRIVATE ${CONAN_LIBS})
