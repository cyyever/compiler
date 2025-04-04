cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)
# set(CMAKE_C_FLAGS -stdlib=libc)
set(CMAKE_CXX_FLAGS -stdlib=libc++)


project(MyCompilerLib LANGUAGES CXX)

include(cmake/all.cmake)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(Boost REQUIRED)
if(NOT MSVC)
  find_package(fmt REQUIRED)
endif()
find_package(CyyAlgorithmLib REQUIRED)
find_package(CyyNaiveLib REQUIRED)
find_package(spdlog REQUIRED)

add_library(MyCompilerLib ${SOURCE})

target_include_directories(
  MyCompilerLib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
                       $<INSTALL_INTERFACE:include>)

find_package(MyComputationLib REQUIRED)
target_link_libraries(MyCompilerLib PRIVATE MyComputationLib CyyAlgorithmLib)

# test
add_subdirectory(test)

option(BUILD_FUZZING "Build fuzzing" OFF)
if(BUILD_FUZZING)
  add_subdirectory(fuzz_test)
endif()

# install lib
install(
  TARGETS MyCompilerLib
  EXPORT MyCompilerLibConfig
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib COMPONENT library
  INCLUDES
  DESTINATION include)

install(
  DIRECTORY src/
  DESTINATION include/cyy/compiler
  FILES_MATCHING
  PATTERN "*.hpp")

# install cmake config file
install(EXPORT MyCompilerLibConfig DESTINATION lib/cmake/MyCompilerLib)
