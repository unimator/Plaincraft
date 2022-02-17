cmake_minimum_required (VERSION 3.20)

project ("Plaincraft")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
# set(BUILD_SHARED_LIBS ON)

find_package(Vulkan)

include_directories(
    SYSTEM "Dependencies/glm"
    SYSTEM "Dependencies/GLEW/include"
    SYSTEM "Dependencies/GLFW/include"
    SYSTEM "Dependencies/ReactPhysics3D/include"
    SYSTEM "Dependencies/imgui"
    SYSTEM ${Vulkan_INCLUDE_DIRS}
    )

add_subdirectory("Dependencies/glm")
add_subdirectory("Dependencies/GLEW")
add_subdirectory("Dependencies/GLFW")
add_subdirectory("Dependencies/ReactPhysics3D")

# Include sub-projects.
add_subdirectory("Common")
add_subdirectory("Core")
add_subdirectory("RenderEngine")
add_subdirectory("RenderEngine_Vulkan")
add_subdirectory("Runner")
add_subdirectory("Dear_ImGui")

