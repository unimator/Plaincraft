add_library("RenderEngine_OpenGL" "")

target_sources("RenderEngine_OpenGL"
PRIVATE
    include/plaincraft_render_engine_opengl.hpp
    src/plaincraft/render_engine_opengl/common.hpp
    src/plaincraft/render_engine_opengl/opengl_render_engine.cpp
    src/plaincraft/render_engine_opengl/opengl_render_engine.hpp
    src/plaincraft/render_engine_opengl/renderer/opengl_renderer.cpp
    src/plaincraft/render_engine_opengl/renderer/opengl_renderer.hpp
    src/plaincraft/render_engine_opengl/shader/opengl_shader.cpp
    src/plaincraft/render_engine_opengl/shader/opengl_shader.hpp
    src/plaincraft/render_engine_opengl/texture/opengl_texture.cpp
    src/plaincraft/render_engine_opengl/texture/opengl_texture.hpp
    src/plaincraft/render_engine_opengl/texture/opengl_textures_factory.cpp
    src/plaincraft/render_engine_opengl/texture/opengl_textures_factory.hpp
)

target_include_directories("RenderEngine_OpenGL" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries("RenderEngine_OpenGL" PRIVATE "Common" "RenderEngine")