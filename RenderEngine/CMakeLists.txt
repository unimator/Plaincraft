add_library("RenderEngine" "")

target_sources("RenderEngine"
PRIVATE
    include/plaincraft_render_engine.hpp
    src/plaincraft/render_engine/camera/camera.cpp
    src/plaincraft/render_engine/camera/camera.hpp
    src/plaincraft/render_engine/renderer/drawable.cpp
    src/plaincraft/render_engine/renderer/drawable.hpp
    src/plaincraft/render_engine/renderer/model.cpp
    src/plaincraft/render_engine/renderer/model.hpp
    src/plaincraft/render_engine/renderer/mvp_matrix.hpp
    src/plaincraft/render_engine/renderer/objects/cube.cpp
    src/plaincraft/render_engine/renderer/objects/cube.hpp
    src/plaincraft/render_engine/renderer/objects/no_draw.cpp
    src/plaincraft/render_engine/renderer/objects/no_draw.hpp
    src/plaincraft/render_engine/renderer/objects/mesh.cpp
    src/plaincraft/render_engine/renderer/objects/mesh.hpp
    src/plaincraft/render_engine/renderer/renderer.cpp
    src/plaincraft/render_engine/renderer/renderer.hpp
    src/plaincraft/render_engine/renderer/vertex.cpp
    src/plaincraft/render_engine/renderer/vertex.hpp
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
