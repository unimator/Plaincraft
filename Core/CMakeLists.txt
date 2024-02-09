#[[
MIT License

This file is part of Plaincraft (https://github.com/unimator/Plaincraft)

Copyright (c) 2020 Marcin Gorka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
]]

set(TARGET_NAME "Core")
add_library(${TARGET_NAME} "")

#[[
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${TARGET_NAME} PRIVATE "-Wall -Wextra")
endif()
if ( MSVC )
    target_compile_options(${TARGET_NAME} PRIVATE "/W4")
endif()
]]

target_sources(${TARGET_NAME}
PRIVATE
    src/plaincraft/core/assets/asset.cpp
    src/plaincraft/core/assets/assets_manager.cpp
    src/plaincraft/core/camera_operators/eyes/camera_operator_eyes.cpp
    src/plaincraft/core/camera_operators/follow/camera_operator_follow.cpp
    src/plaincraft/core/camera_operators/camera_operator.cpp
    src/plaincraft/core/entities/blocks/block.cpp
    src/plaincraft/core/entities/blocks/dirt.cpp
    src/plaincraft/core/entities/blocks/stone.cpp
    src/plaincraft/core/entities/map/chunk.cpp
    src/plaincraft/core/entities/map/map.cpp
    src/plaincraft/core/entities/player/events/player_events_handler.cpp
    src/plaincraft/core/entities/player/player.cpp
    src/plaincraft/core/entities/game_object.cpp
    src/plaincraft/core/events/loop_events_handler.cpp
    src/plaincraft/core/initialization/scene_builder.cpp
    src/plaincraft/core/physics/physics_engine.cpp
    src/plaincraft/core/physics/physics_object.cpp
    src/plaincraft/core/physics_optimization/active_objects_optimizer.cpp
    src/plaincraft/core/physics_optimization/map_optimizer.cpp
    src/plaincraft/core/scene/scene.cpp
    src/plaincraft/core/utils/conversions.cpp
    src/plaincraft/core/utils/fps_counter.cpp
    src/plaincraft/core/state/global_state.cpp
    src/plaincraft/core/world/chunks/chunk_builder_base.cpp
    src/plaincraft/core/world/chunks/chunk_builder.cpp
    src/plaincraft/core/world/chunks/chunks_processor.cpp
    src/plaincraft/core/world/chunks/simple_chunk_builder.cpp
    src/plaincraft/core/world/world_generator.cpp
    src/plaincraft/core/world/world_optimizer.cpp
    src/plaincraft/core/game.cpp
)

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${TARGET_NAME} 
                        PUBLIC "Common" "RenderEngine" 
                        PRIVATE "RenderEngine_Vulkan")
