add_library("ImGui_Vulkan" "")

target_sources("ImGui_Vulkan" 
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/imgui_widgets.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/backends/imgui_impl_vulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/imgui/backends/imgui_impl_glfw.cpp"
)