cmake_minimum_required(VERSION 3.15)

project(vklearn)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
# find_package(tinyobjloader REQUIRED)
# find_package(imgui REQUIRED)

#include_directories(${Vulkan_INCLUDE_DIRS})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Executable
#file(GLOB SOURCES src/*.cpp)
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/vertex.cpp
  src/application.cpp
  src/model.cpp
  src/camera.cpp
  src/vulkan_context.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Vulkan_INCLUDE_DIRS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  GLFW_INCLUDE_VULKAN
  GLM_FORCE_RADIANS
  GLM_FORCE_DEPTH_ZERO_TO_ONE
  GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(${PROJECT_NAME}
  Vulkan::Vulkan
  glfw
  glm
  #tinyobjloader
  #imgui
)

