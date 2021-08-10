cmake_minimum_required (VERSION 3.20)

project ("Plaincraft")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
# set(BUILD_SHARED_LIBS ON)

find_package(Vulkan)

include_directories(
    "Dependencies/glm"
    "Dependencies/GLEW/include"
    "Dependencies/GLFW/include"
    ${Vulkan_INCLUDE_DIRS}
    )

add_subdirectory("Dependencies/glm")
add_subdirectory("Dependencies/GLEW")
add_subdirectory("Dependencies/GLFW")

# Include sub-projects.
add_subdirectory("Common")
add_subdirectory("Core")
add_subdirectory("RenderEngine")
add_subdirectory("RenderEngine_OpenGL")
add_subdirectory("RenderEngine_Vulkan")
add_subdirectory("Runner")


