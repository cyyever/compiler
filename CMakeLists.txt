CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(MyCompilerLib LANGUAGES CXX)

include(cmake/all.cmake)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)

find_package(range-v3 REQUIRED)

add_library(MyCompilerLib ${SOURCE})

target_link_libraries(MyCompilerLib PUBLIC range-v3)

find_package(MyComputationLib REQUIRED)
target_link_libraries(MyCompilerLib PRIVATE MyComputationLib)

# test
add_subdirectory(test)

option(BUILD_FUZZING "Build fuzzing" ON)

if(BUILD_FUZZING)
  add_subdirectory(fuzz_test)
endif()

# install lib
install(TARGETS MyCompilerLib EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/cyy/compiler
  COMPONENT library
  INCLUDES DESTINATION include
  )

install(DIRECTORY src/ DESTINATION include/cyy/compiler
  FILES_MATCHING PATTERN "*.hpp")

# install cmake config file
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Config.cmake
  DESTINATION lib/cmake/${PROJECT_NAME})
