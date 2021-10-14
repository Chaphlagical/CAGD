cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(CAGD VERSION 0.0.1)
message(STATUS "[Project] ${PROJECT_NAME} ${PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(Init)
Init_Project()

find_package(OpenMP REQUIRED)

add_subdirectory(Base)
add_subdirectory(Framework)
add_subdirectory(Homeworks)
