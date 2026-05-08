cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

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

# PackageProject.cmake will be used to make our target installable
option(BUILD_SHARED_LIBS "Build shared libraries" NO)

if(NOT CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/_deps" CACHE PATH
          "Directory to share CPM dependency sources across build trees" FORCE)
  set(ENV{CPM_SOURCE_CACHE} ${CPM_SOURCE_CACHE})
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_SOURCE_CACHE $ENV{CPM_SOURCE_CACHE} CACHE PATH
          "Directory to share CPM dependency sources across build trees" FORCE)
elseif(NOT CPM_SOURCE_CACHE)
  set(ENV{CPM_SOURCE_CACHE} ${CPM_SOURCE_CACHE})
endif()
if(NOT EXISTS ${CPM_SOURCE_CACHE})
  file(MAKE_DIRECTORY ${CPM_SOURCE_CACHE})
endif()
message("CPM_SOURCE_CACHE: ${CPM_SOURCE_CACHE}")

include(cmake/CPM.cmake)

if(NOT DEFINED ${FETCHCONTENT_SOURCE_DIR_BOOST})
  message("FETCHCONTENT_SOURCE_DIR_BOOST not defined")
  set(_fznparser_boost_source_dir ${CPM_SOURCE_CACHE}/boost_1_80_0)
  if(NOT EXISTS "${_atlantis_boost_source_dir}/boost/version.hpp")
    set(BOOST_URL https://sourceforge.net/projects/boost/files/boost/1.80.0/boost_1_80_0.7z)
    set(BOOST_URL_SHA256 d12a2af721e22dbfb984ef061ee4c4ab2387f1904f6d65bc5acebaa34d6366ec)

    set(_fznparser_boost_archive "${CPM_SOURCE_CACHE}/boost_1_80_0.tar.bz2")
    if(EXISTS "${_fznparser_boost_archive}")
      file(SHA256 "${_fznparser_boost_archive}" _fznparser_boost_archive_hash)
      if(NOT _fznparser_boost_archive_hash STREQUAL BOOST_URL_SHA256)
        file(REMOVE "${_fznparser_boost_archive}")
      endif()
    endif()

    file(MAKE_DIRECTORY "${_fznparser_boost_source_dir}")
    file(DOWNLOAD
            "${BOOST_URL}"
            "${_fznparser_boost_archive}"
            EXPECTED_HASH "SHA256=${BOOST_URL_SHA256}"
            SHOW_PROGRESS)

    set(_fznparser_boost_extract_dir "${_fznparser_boost_source_dir}.extract")
    file(REMOVE_RECURSE "${_fznparser_boost_extract_dir}")
    file(MAKE_DIRECTORY "${_fznparser_boost_extract_dir}")
    execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar xjf "${_fznparser_boost_archive}"
            WORKING_DIRECTORY "${_fznparser_boost_extract_dir}"
            RESULT_VARIABLE _fznparser_boost_extract_result)
    if(NOT _fznparser_boost_extract_result EQUAL 0)
      message(FATAL_ERROR "Failed to extract cached Boost archive: ${_fznparser_boost_archive}")
    endif()

    file(REMOVE_RECURSE "${_fznparser_boost_source_dir}")
    file(RENAME
            "${_fznparser_boost_extract_dir}/boost_1_80_0"
            "${_fznparser_boost_source_dir}")
    file(REMOVE_RECURSE "${_fznparser_boost_extract_dir}")
  endif()
  set(FETCHCONTENT_SOURCE_DIR_BOOST ${_fznparser_boost_source_dir})
endif()

# add_compile_definitions(BOOST_NO_CXX98_FUNCTION_BASE=TRUE)
CPMAddPackage(
  NAME boost-cmake
  GITHUB_REPOSITORY "ClausKlein/boost-cmake"
  GIT_TAG "v1.80.0"
)

CPMAddPackage(
  NAME PackageProject.cmake
  GITHUB_REPOSITORY "TheLartians/PackageProject.cmake"
  GIT_TAG "v1.13.0"
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
