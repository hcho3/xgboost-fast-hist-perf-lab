cmake_minimum_required(VERSION 3.2)
project(xgboost-fast-hist-perf-lab C CXX)
include(cmake/OpenMP.cmake)
include(cmake/Utils.cmake)

set_default_configuration_release()
msvc_use_static_runtime()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB SOURCE "src/*.cc")

add_executable(perflab ${SOURCE})
target_link_libraries(perflab PUBLIC OpenMP::OpenMP_CXX)
target_include_directories(perflab PRIVATE ${PROJECT_SOURCE_DIR}/include)

set_output_directory(perflab ${PROJECT_SOURCE_DIR})
