add_library("RenderEngine" "")

target_sources("RenderEngine"
PRIVATE
    include/plaincraft_render_engine.hpp
    src/plaincraft/render_engine/camera/camera.cpp
    src/plaincraft/render_engine/camera/camera.hpp
    src/plaincraft/render_engine/events/window_events_handler.cpp
    src/plaincraft/render_engine/events/window_events_handler.hpp
    src/plaincraft/render_engine/gui/font/font.hpp
    src/plaincraft/render_engine/gui/font/fonts_factory.hpp
    src/plaincraft/render_engine/gui/menu/menu_button.cpp
    src/plaincraft/render_engine/gui/menu/menu_button.hpp
    src/plaincraft/render_engine/gui/menu/menu_factory.cpp
    src/plaincraft/render_engine/gui/menu/menu_factory.hpp
    src/plaincraft/render_engine/gui/menu/menu.cpp
    src/plaincraft/render_engine/gui/menu/menu.hpp
    src/plaincraft/render_engine/gui/gui_renderer.cpp
    src/plaincraft/render_engine/gui/gui_renderer.hpp
    src/plaincraft/render_engine/gui/gui_widget.cpp
    src/plaincraft/render_engine/gui/gui_widget.hpp
    src/plaincraft/render_engine/models/model.cpp
    src/plaincraft/render_engine/models/model.hpp
    src/plaincraft/render_engine/models/models_factory.cpp
    src/plaincraft/render_engine/models/models_factory.hpp
    src/plaincraft/render_engine/scene/objects/cube.cpp
    src/plaincraft/render_engine/scene/objects/cube.hpp
    src/plaincraft/render_engine/scene/objects/mesh.cpp
    src/plaincraft/render_engine/scene/objects/mesh.hpp
    src/plaincraft/render_engine/scene/objects/no_draw.cpp
    src/plaincraft/render_engine/scene/objects/no_draw.hpp
    src/plaincraft/render_engine/scene/drawable.cpp
    src/plaincraft/render_engine/scene/drawable.hpp
    src/plaincraft/render_engine/scene/mvp_matrix.hpp
    src/plaincraft/render_engine/scene/scene_renderer.cpp
    src/plaincraft/render_engine/scene/scene_renderer.hpp
    src/plaincraft/render_engine/scene/vertex.cpp
    src/plaincraft/render_engine/scene/vertex.hpp
    src/plaincraft/render_engine/texture/texture.hpp
    src/plaincraft/render_engine/texture/textures_factory.hpp
    src/plaincraft/render_engine/texture/textures_repository.cpp
    src/plaincraft/render_engine/texture/textures_repository.hpp
    src/plaincraft/render_engine/utils/image/image.cpp
    src/plaincraft/render_engine/utils/image/image.hpp
    src/plaincraft/render_engine/window/window.cpp
    src/plaincraft/render_engine/window/window.hpp
    src/plaincraft/render_engine/common.hpp
    src/plaincraft/render_engine/frame_config.hpp
    src/plaincraft/render_engine/render_engine.cpp
    src/plaincraft/render_engine/render_engine.hpp
)

target_include_directories("RenderEngine" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries("RenderEngine" PRIVATE "Common")
