# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.

# Name of the project
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_NAME fznparser)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  # If we are debugging then add debugging flag
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g2 -O0")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g0 -O3 -DNDEBUG ")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#----------------------------------------------------------------------------
# Setup ANTLR for generating the FlatZinc parser.
#----------------------------------------------------------------------------
# Required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
# Use the latest release of 4.9.3
set(ANTLR4_TAG e4c1a74)
# Set policy
cmake_policy(SET CMP0114 NEW)
# Add external build for antlrcpp
include(ExternalAntlr4Cpp)
# Add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
# set(ANTLR_EXECUTABLE /usr/local/lib/antlr-4.9.1-complete.jar)
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/thirdparty/antlr/antlr-4.9.1-complete.jar)

# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(Sample ${PROJECT_SOURCE_DIR}/src/FlatZinc.g4
  PACKAGE fznparser
  VISITOR)

# include generated files in project environment
include_directories(${ANTLR_Sample_OUTPUT_DIR})

#----------------------------------------------------------------------------
# Define project sources and includes
#----------------------------------------------------------------------------

# Retrieve all cpp, hpp, and h files in ./src
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
# Set the 'library' name. The library consists of all cpp, hpp, and h files except
# main in ./src
set(PROJECT_LIB ${PROJECT_NAME}_lib)
# includes (hpp/h) files are in ./include
include_directories(${COMMON_INCLUDES})

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hpp)
# remove main from the src files
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/FznVisitor.cpp)

# The src files make up the library
add_library(${PROJECT_LIB} ${SRC_FILES} ${ANTLR_FlatzincParser_CXX_OUTPUTS})
target_link_libraries(${PROJECT_LIB} antlr4_static)

add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIB} antlr4_static)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts
  GIT_TAG v3.0.0
)
FetchContent_MakeAvailable(cxxopts)
include_directories(${cxxopts_SOURCE_DIR}/include)

if (BUILD_TESTS)
  # Google Test is required for testing and benchmarking
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  ################################
  # Testing
  ################################

  # Required for rapidcheck
  set(RC_ENABLE_GTEST ON CACHE BOOL "Rapidcheck GTest Support" FORCE)
  set(RC_ENABLE_GMOCK ON CACHE BOOL "Rapidcheck GMock Support" FORCE)
  set(RC_ENABLE_BOOST ON CACHE BOOL "Rapidcheck Boost Support" FORCE)
  set(RC_ENABLE_RTTI  ON CACHE BOOL "Rapidcheck RTTI Support"  FORCE)

  FetchContent_Declare(rapidcheck
    GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
    GIT_TAG 33d15a858e3125f5af61a655f390f1cbc2f272ba
  )

  FetchContent_MakeAvailable(googletest rapidcheck)

  include(CTest)

  enable_testing()

  ##############
  # Unit Tests
  ##############
  file(GLOB_RECURSE TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/test/*.h ${PROJECT_SOURCE_DIR}/test/*.hpp)
  add_executable(runUnitTests ${TEST_SRC_FILES})

  # Standard linking to gtest, gmock, and rapidcheck.
  target_link_libraries(
    runUnitTests
    gtest
    gtest_main
    gmock
    gmock_main
    rapidcheck_gtest
    rapidcheck_gmock
    Threads::Threads
    -lm
    ${PROJECT_LIB}
  )

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  # add_test(NAME "UnitTests" COMMAND "runUnitTests")

  set_target_properties(runUnitTests PROPERTIES FOLDER tests)
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # This is for Windows
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})