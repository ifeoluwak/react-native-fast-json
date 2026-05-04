project(fastjson)
cmake_minimum_required(VERSION 3.9.0)

set(PACKAGE_NAME fastjson)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

# Define C++ library and add all sources
add_library(
        ${PACKAGE_NAME}
        SHARED
        src/main/cpp/cpp-adapter.cpp
        ../cpp/third_party/simdjson.cpp
)

# Add Nitrogen specs :)
include(${CMAKE_SOURCE_DIR}/../nitrogen/generated/android/fastjson+autolinking.cmake)

# simdjson amalgamation + project C++
target_include_directories(
        ${PACKAGE_NAME}
        PRIVATE
        src/main/cpp
        ../cpp
        ../cpp/third_party
)

find_library(LOG_LIB log)

# Link all libraries together
target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
        android # <-- Android core
)
