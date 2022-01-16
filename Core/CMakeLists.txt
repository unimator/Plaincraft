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
    include/plaincraft_core.hpp
    src/plaincraft/core/common.hpp
    src/plaincraft/core/camera_operators/eyes/camera_operator_eyes.cpp
    src/plaincraft/core/camera_operators/eyes/camera_operator_eyes.hpp
    src/plaincraft/core/camera_operators/follow/camera_operator_follow.cpp
    src/plaincraft/core/camera_operators/follow/camera_operator_follow.hpp
    src/plaincraft/core/camera_operators/camera_operator.cpp
    src/plaincraft/core/camera_operators/camera_operator.hpp
    src/plaincraft/core/components/component.hpp
    src/plaincraft/core/entities/blocks/block.cpp
    src/plaincraft/core/entities/blocks/block.hpp
    src/plaincraft/core/entities/blocks/dirt.cpp
    src/plaincraft/core/entities/blocks/dirt.hpp
    src/plaincraft/core/entities/map/chunk.cpp
    src/plaincraft/core/entities/map/chunk.hpp
    src/plaincraft/core/entities/map/map.cpp
    src/plaincraft/core/entities/map/map.hpp
    src/plaincraft/core/entities/player/events/player_events_handler.cpp
    src/plaincraft/core/entities/player/events/player_events_handler.hpp
    src/plaincraft/core/entities/player/player.cpp
    src/plaincraft/core/entities/player/player.hpp
    src/plaincraft/core/entities/entity.cpp
    src/plaincraft/core/entities/entity.hpp
    src/plaincraft/core/events/loop_events_handler.cpp
    src/plaincraft/core/events/loop_events_handler.hpp
    src/plaincraft/core/models/models_cache.cpp
    src/plaincraft/core/models/models_cache.hpp
    src/plaincraft/core/utils/conversions.cpp
    src/plaincraft/core/utils/conversions.hpp
    src/plaincraft/core/world/world_generator.cpp
    src/plaincraft/core/world/world_generator.hpp
    src/plaincraft/core/game.cpp
    src/plaincraft/core/game.hpp
    src/plaincraft/core/scene.cpp
    src/plaincraft/core/scene.hpp
    
)

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${TARGET_NAME} 
                        PUBLIC "Common" "RenderEngine" 
                        PRIVATE "RenderEngine_OpenGL" "RenderEngine_Vulkan" "reactphysics3d")
