add_library("RenderEngine_Vulkan" "")

target_sources("RenderEngine_Vulkan"
PRIVATE
    include/plaincraft_render_engine_vulkan.hpp
    src/plaincraft/render_engine_vulkan/common.hpp
    src/plaincraft/render_engine_vulkan/queue_family_indices.hpp
    src/plaincraft/render_engine_vulkan/renderer/vertex_utils.cpp
    src/plaincraft/render_engine_vulkan/renderer/vertex_utils.hpp
    src/plaincraft/render_engine_vulkan/renderer/vulkan_renderer.cpp
    src/plaincraft/render_engine_vulkan/renderer/vulkan_renderer.hpp
    src/plaincraft/render_engine_vulkan/shader/vulkan_shader.cpp
    src/plaincraft/render_engine_vulkan/shader/vulkan_shader.hpp
    src/plaincraft/render_engine_vulkan/swap_chain_support_details.hpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_texture.cpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_texture.hpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_textures_factory.cpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_textures_factory.hpp
    src/plaincraft/render_engine_vulkan/vulkan_render_engine.cpp
    src/plaincraft/render_engine_vulkan/vulkan_render_engine.hpp
)

find_package(Vulkan)

target_include_directories("RenderEngine_Vulkan" PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories("RenderEngine_Vulkan" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries("RenderEngine_Vulkan" PRIVATE "Common" "RenderEngine")
target_link_libraries("RenderEngine_Vulkan" PUBLIC ${Vulkan_LIBRARIES})