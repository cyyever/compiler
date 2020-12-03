cmake_minimum_required(VERSION 3.13)

project(MyCompilerLib LANGUAGES CXX)

include(cmake/all.cmake)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)

find_package(Boost REQUIRED)

add_library(MyCompilerLib ${SOURCE})

target_include_directories(
  MyCompilerLib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
                       $<INSTALL_INTERFACE:include>)

find_package(MyComputationLib REQUIRED)
target_link_libraries(MyCompilerLib PRIVATE MyComputationLib)

if(MSVC)
  find_package(MicrosoftSTL REQUIRED)
  target_link_libraries(MyCompilerLib PUBLIC Microsoft::STL)
endif()

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
