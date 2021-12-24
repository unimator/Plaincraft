add_library("RenderEngine" "")

target_sources("RenderEngine"
PRIVATE
    include/plaincraft_render_engine.hpp
    src/plaincraft/render_engine/camera/camera.cpp
    src/plaincraft/render_engine/camera/camera.hpp
    src/plaincraft/render_engine/events/window_events_handler.cpp
    src/plaincraft/render_engine/events/window_events_handler.hpp
    src/plaincraft/render_engine/models/model.cpp
    src/plaincraft/render_engine/models/model.hpp
    src/plaincraft/render_engine/models/models_factory.cpp
    src/plaincraft/render_engine/models/models_factory.hpp
    src/plaincraft/render_engine/scene_rendering/objects/cube.cpp
    src/plaincraft/render_engine/scene_rendering/objects/cube.hpp
    src/plaincraft/render_engine/scene_rendering/objects/mesh.cpp
    src/plaincraft/render_engine/scene_rendering/objects/mesh.hpp
    src/plaincraft/render_engine/scene_rendering/objects/no_draw.cpp
    src/plaincraft/render_engine/scene_rendering/objects/no_draw.hpp
    src/plaincraft/render_engine/scene_rendering/drawable.cpp
    src/plaincraft/render_engine/scene_rendering/drawable.hpp
    src/plaincraft/render_engine/scene_rendering/mvp_matrix.hpp
    src/plaincraft/render_engine/scene_rendering/scene_renderer.cpp
    src/plaincraft/render_engine/scene_rendering/scene_renderer.hpp
    src/plaincraft/render_engine/scene_rendering/vertex.cpp
    src/plaincraft/render_engine/scene_rendering/vertex.hpp
    src/plaincraft/render_engine/texture/texture.hpp
    src/plaincraft/render_engine/texture/textures_factory.hpp
    src/plaincraft/render_engine/texture/textures_repository.cpp
    src/plaincraft/render_engine/texture/textures_repository.hpp
    src/plaincraft/render_engine/utils/image/image.cpp
    src/plaincraft/render_engine/utils/image/image.hpp
    src/plaincraft/render_engine/window/window.cpp
    src/plaincraft/render_engine/window/window.hpp
    src/plaincraft/render_engine/common.hpp
    src/plaincraft/render_engine/render_engine.cpp
    src/plaincraft/render_engine/render_engine.hpp
)

target_include_directories("RenderEngine" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries("RenderEngine" PRIVATE "Common")
