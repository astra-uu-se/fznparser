cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PROJECT_NAME fznparserTests)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ENV{CPM_SOURCE_CACHE} "${CMAKE_BINARY_DIR}/_deps")
message("CPM_SOURCE_CACHE $ENV{CPM_SOURCE_CACHE}")

enable_testing()

# ---- Dependencies ----
message("Downloading CPM")
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)
message("Including CPM ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake")
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

message("CPM_SOURCE_CACHE $ENV{CPM_SOURCE_CACHE}")

message("CPM adding googletest")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

CPMAddPackage(NAME fznparser SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

message("CPM_SOURCE_CACHE $ENV{CPM_SOURCE_CACHE}")

# ---- Create binary ----
# Add a definition for the directory that contains the test stubs.
add_compile_definitions(STUB_DIR="${CMAKE_CURRENT_SOURCE_DIR}/stubs")
add_compile_definitions(FZN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fzn-models")
add_compile_definitions(FZN_CHALLENGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/flattened-mzn-challenge")

# #############
# Unit Tests
# #############
file(GLOB_RECURSE
  TEST_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_executable(${PROJECT_NAME} ${TEST_SRC_FILES})

target_link_libraries(${PROJECT_NAME} GTest::gtest_main fznparser::fznparser)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER tests)

get_target_property(PRIVATE_INCLUDE_DIRS fznparser::fznparser INCLUDE_DIRECTORIES)

target_include_directories(${PROJECT_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})

include(GoogleTest)
gtest_add_tests(${PROJECT_NAME} SOURCES ${TEST_SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(fznparser
    PUBLIC -std=c++20 -Wall -Wpedantic -Wextra -Werror)
elseif(MSVC)
  target_compile_options(fznparser PUBLIC /W4 /WX)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
endif()