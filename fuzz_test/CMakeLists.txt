CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
PROJECT(ComplierFuzzTest)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../cmake/fuzzing.cmake)

FILE(GLOB test_sources 
  ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp
  )

FOREACH(test_source ${test_sources})
  GET_FILENAME_COMPONENT(test_prog ${test_source} NAME_WE)
  ADD_EXECUTABLE(${test_prog} ${test_source})
  TARGET_LINK_LIBRARIES(${test_prog} PRIVATE MyComputationLib)
  TARGET_LINK_LIBRARIES(${test_prog} PRIVATE MyCompilerLib)
  add_fuzzing(TARGET ${test_prog})
ENDFOREACH()
