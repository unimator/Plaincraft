add_library("RenderEngine_Vulkan" "")

target_sources("RenderEngine_Vulkan"
PRIVATE
    include/plaincraft_render_engine_vulkan.hpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_pool.cpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_pool.hpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_set_layout.cpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_set_layout.hpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_writer.cpp
    src/plaincraft/render_engine_vulkan/descriptors/vulkan_descriptor_writer.hpp
    src/plaincraft/render_engine_vulkan/device/vulkan_device.cpp
    src/plaincraft/render_engine_vulkan/device/vulkan_device.hpp
    src/plaincraft/render_engine_vulkan/instance/vulkan_instance_config.cpp
    src/plaincraft/render_engine_vulkan/instance/vulkan_instance_config.hpp
    src/plaincraft/render_engine_vulkan/instance/vulkan_instance.cpp
    src/plaincraft/render_engine_vulkan/instance/vulkan_instance.hpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_buffer.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_buffer.hpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_image_view.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_image_view.hpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_image.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_image.hpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_texture.cpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_texture.hpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_uniform_buffer.hpp
    src/plaincraft/render_engine_vulkan/memory/vulkan_uniform_buffer.cpp
    src/plaincraft/render_engine_vulkan/models/vulkan_model.cpp
    src/plaincraft/render_engine_vulkan/models/vulkan_model.hpp
    src/plaincraft/render_engine_vulkan/models/vulkan_models_factory.cpp
    src/plaincraft/render_engine_vulkan/models/vulkan_models_factory.hpp
    src/plaincraft/render_engine_vulkan/pipeline/vulkan_pipeline_config.hpp
    src/plaincraft/render_engine_vulkan/pipeline/vulkan_pipeline.hpp
    src/plaincraft/render_engine_vulkan/pipeline/vulkan_pipeline.cpp
    src/plaincraft/render_engine_vulkan/rendering/gui/windows/diagnostic/vulkan_diagnostic_widget.cpp
    src/plaincraft/render_engine_vulkan/rendering/gui/windows/diagnostic/vulkan_diagnostic_widget.hpp
    src/plaincraft/render_engine_vulkan/rendering/gui/vulkan_gui_renderer.cpp
    src/plaincraft/render_engine_vulkan/rendering/gui/vulkan_gui_renderer.hpp
    src/plaincraft/render_engine_vulkan/rendering/gui/vulkan_gui_widget.cpp
    src/plaincraft/render_engine_vulkan/rendering/gui/vulkan_gui_widget.hpp
    src/plaincraft/render_engine_vulkan/rendering/scene/vertex_utils.cpp
    src/plaincraft/render_engine_vulkan/rendering/scene/vertex_utils.hpp
    src/plaincraft/render_engine_vulkan/rendering/scene/vulkan_drawable.hpp
    src/plaincraft/render_engine_vulkan/rendering/scene/vulkan_scene_renderer_config.hpp
    src/plaincraft/render_engine_vulkan/rendering/scene/vulkan_scene_renderer.cpp
    src/plaincraft/render_engine_vulkan/rendering/scene/vulkan_scene_renderer.hpp
    src/plaincraft/render_engine_vulkan/rendering/vulkan_renderer_frame_config.hpp
    src/plaincraft/render_engine_vulkan/shader/vulkan_shader.cpp
    src/plaincraft/render_engine_vulkan/shader/vulkan_shader.hpp
    src/plaincraft/render_engine_vulkan/swapchain/vulkan_swapchain.cpp
    src/plaincraft/render_engine_vulkan/swapchain/vulkan_swapchain.hpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_texture.cpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_texture.hpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_textures_factory.cpp
    src/plaincraft/render_engine_vulkan/texture/vulkan_textures_factory.hpp
    src/plaincraft/render_engine_vulkan/utils/device_extensions.hpp
    src/plaincraft/render_engine_vulkan/utils/queue_family.cpp
    src/plaincraft/render_engine_vulkan/utils/queue_family.hpp
    src/plaincraft/render_engine_vulkan/utils/validation_layers.hpp
    src/plaincraft/render_engine_vulkan/window/vulkan_window.hpp
    src/plaincraft/render_engine_vulkan/window/vulkan_window.cpp
    src/plaincraft/render_engine_vulkan/common.hpp
    src/plaincraft/render_engine_vulkan/vulkan_render_engine.cpp
    src/plaincraft/render_engine_vulkan/vulkan_render_engine.hpp
)

find_package(Vulkan)

target_include_directories("RenderEngine_Vulkan" PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories("RenderEngine_Vulkan" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries("RenderEngine_Vulkan" PRIVATE "Common" "RenderEngine" "Dear_ImGui_Vulkan")
target_link_libraries("RenderEngine_Vulkan" PUBLIC ${Vulkan_LIBRARIES})
