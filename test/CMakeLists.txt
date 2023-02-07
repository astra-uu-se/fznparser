cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(fznparserTests LANGUAGES CXX)

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

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} PRIVATE gtest fznparser::fznparser)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(fznparser PUBLIC -Wall -Wpedantic -Wextra -Werror)
elseif(MSVC)
  target_compile_options(fznparser PUBLIC /W4 /WX)
  target_compile_definitions(${PROJECT_NAME} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
endif()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
