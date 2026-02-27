cmake_minimum_required(VERSION 4.2)

project(LearningOpengl VERSION 1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(
  opengltut
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)


add_custom_target(
  run
  COMMAND $<TARGET_FILE:opengltut> 
  DEPENDS opengltut
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running application"
)


target_include_directories(opengltut PRIVATE
    ${CMAKE_SOURCE_DIR}/../Include
)

target_link_directories(opengltut PRIVATE
    ${CMAKE_SOURCE_DIR}/../Libs
)

target_link_libraries(opengltut PRIVATE
  glfw3
  GL
  dl
  pthread
  X11
)
