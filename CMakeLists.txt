cmake_minimum_required(VERSION 3.20.. FATAL_ERROR)

set(META_LIBRARY_NAME "fznparser")
set(META_PROJECT_VERSION_FULL "0.2")
set(META_PROJECT_DESCRIPTION "FlatZinc parser")

# ---- Project ----
project(
  fznparser
  VERSION ${META_PROJECT_VERSION_FULL}
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
    "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
option(BUILD_SHARED_LIBS "Build shared libraries" NO)

find_program(HAS_CURL curl)

if(NOT HAS_CURL STREQUAL "HAS_CURL-NOTFOUND")
  message("curl found at ${HAS_CURL}")
  execute_process(
    COMMAND ${HAS_CURL} -so /dev/null -I --head --fail https://landing.jfrog.com/reactivate-server/boostorg
    RESULT_VARIABLE BOOST_URL_NO_CONNECTION)

  if(NOT BOOST_URL_NO_CONNECTION STREQUAL '200')
    message("The boost URL is not available, switching to sourceforge")
    set(BOOST_URL https://sourceforge.net/projects/boost/files/boost/1.80.0/boost_1_80_0.7z)
    set(BOOST_URL_SHA256 d12a2af721e22dbfb984ef061ee4c4ab2387f1904f6d65bc5acebaa34d6366ec)
  endif()
endif()

# add_compile_definitions(BOOST_NO_CXX98_FUNCTION_BASE=TRUE)
CPMAddPackage(
  NAME boost-cmake
  GITHUB_REPOSITORY "ClausKlein/boost-cmake"
  GIT_TAG "v1.80.0"
)

CPMAddPackage(
  NAME PackageProject.cmake
  VERSION 1.10.0
  GITHUB_REPOSITORY "TheLartians/PackageProject.cmake"
  GIT_TAG "v1.10.0"
)

# ---- Add source files ----
# Configure version header
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
configure_file(${PROJECT_SOURCE_DIR}/config.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/include/config.hpp @ONLY)

# Note: globbing sources is considered bad practice as CMake's generators may
# not detect new files automatically. Keep that in mind when changing files, or
# explicitly mention them here.
file(GLOB_RECURSE
  HEADERS_PUBLIC
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

file(GLOB_RECURSE
  HEADERS_PRIVATE
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/include_private/*.hpp")

file(GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

# ---- Create library ----
add_library(${PROJECT_NAME}
  ${SOURCES}
  ${HEADERS_PUBLIC}
  ${HEADERS_PRIVATE})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME}
  PRIVATE
  -std=c++20
  -Wall
  -Wextra
  -Wnon-virtual-dtor
  -pedantic
  PUBLIC
  "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# Link dependencies
target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${GENERATED_HEADER_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  ${PROJECT_SOURCE_DIR}/include_private)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Boost::asio
)

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match
# the project's regular header paths
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  DEPENDENCIES "Boost 1.79"
)
