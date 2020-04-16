cmake_minimum_required(VERSION 3.13)

project(MyCompilerLib LANGUAGES CXX)

include(cmake/all.cmake)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)

find_package(range-v3 REQUIRED)
find_package(Boost REQUIRED)

add_library(MyCompilerLib ${SOURCE})

target_link_libraries(MyCompilerLib PUBLIC range-v3)

find_package(MyComputationLib REQUIRED)
target_link_libraries(MyCompilerLib PRIVATE MyComputationLib)

find_package(MicrosoftGSL REQUIRED)
target_link_libraries(MyCompilerLib PUBLIC Microsoft::GSL)
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  find_package(libcxx REQUIRED)
  target_link_libraries(MyCompilerLib PUBLIC libcxx::libcxx)
elseif(MSVC)
  find_package(MicrosoftSTL REQUIRED)
  target_link_libraries(MyCompilerLib PUBLIC Microsoft::STL)
endif()

# test
add_subdirectory(test)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  option(BUILD_FUZZING "Build fuzzing" ON)
else()
  option(BUILD_FUZZING "Build fuzzing" OFF)
endif()

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
install(EXPORT MyCompilerLibConfig DESTINATION lib/cmake/${PROJECT_NAME})
