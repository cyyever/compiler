CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(MyCompilerLib LANGUAGES CXX)

include(cmake/all.cmake)

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)
file(GLOB HRDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.hpp)

find_package(MicrosoftGSL REQUIRED)

add_library(MyCompilerLib ${SOURCE})

target_link_libraries(MyCompilerLib PUBLIC MicrosoftGSL::GSL)

set_target_properties(MyCompilerLib PROPERTIES PUBLIC_HEADER "${HRDS}")

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

# install cmake config file
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Config.cmake
  DESTINATION lib/cmake/${PROJECT_NAME})
