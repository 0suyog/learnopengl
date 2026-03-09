cmake_minimum_required(VERSION 4.2)

project(
  LearningOpengl
  VERSION 1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(common INTERFACE)

target_include_directories(common INTERFACE ${CMAKE_SOURCE_DIR}/../Include)

target_link_directories(common INTERFACE ${CMAKE_SOURCE_DIR}/../Libs)

target_link_libraries(common INTERFACE glfw3 GL dl pthread X11)

add_executable(opengltut ${CMAKE_CURRENT_SOURCE_DIR}/glad.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(opengltut PRIVATE common)
add_executable(5_8_1 ${CMAKE_CURRENT_SOURCE_DIR}/exercise/5_8_1.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/glad.cpp)
target_link_libraries(5_8_1 PRIVATE common)

add_custom_target(
  exec5_8_1
  COMMAND $<TARGET_FILE:5_8_1>
  DEPENDS 5_8_1
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running exercise 5_8_1")

add_custom_target(
  run
  COMMAND $<TARGET_FILE:opengltut>
  DEPENDS opengltut
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running application")

add_executable(5_8_2 ${CMAKE_CURRENT_SOURCE_DIR}/exercise/5_8_2.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/glad.cpp)
target_link_libraries(5_8_2 PRIVATE common)

add_custom_target(
  exec5_8_2
  COMMAND $<TARGET_FILE:5_8_2>
  DEPENDS 5_8_2
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running exercise 5_8_2")
