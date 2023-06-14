cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(fznparserTests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

# ---- Dependencies ----
include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.11.0
  OPTIONS
  "INSTALL_GTEST OFF"
  "gtest_force_shared_crt ON"
)

CPMAddPackage(NAME fznparser SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create binary ----
# Add a definition for the directory that contains the test stubs.
add_compile_definitions(STUB_DIR="${CMAKE_CURRENT_SOURCE_DIR}/stubs")
add_compile_definitions(FZN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../fzn-models")

file(GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE fznparser::fznparser gtest)

get_target_property(PRIVATE_INCLUDE_DIRS fznparser::fznparser INCLUDE_DIRECTORIES)

target_include_directories(${PROJECT_NAME} PRIVATE ${PRIVATE_INCLUDE_DIRS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(fznparser
    PUBLIC -std=c++20 -Wall -Wpedantic -Wextra -Werror)
elseif(MSVC)
  target_compile_options(fznparser PUBLIC /W4 /WX)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
endif()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
