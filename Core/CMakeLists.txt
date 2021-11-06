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
    src/plaincraft/core/entities/entity.cpp
    src/plaincraft/core/entities/entity.hpp
    src/plaincraft/core/entities/player.cpp
    src/plaincraft/core/entities/player.hpp
    src/plaincraft/core/events/event.hpp
    src/plaincraft/core/events/events_manager.cpp
    src/plaincraft/core/events/events_manager.hpp
    src/plaincraft/core/events/event_observer.hpp
    src/plaincraft/core/events/event_trigger.hpp
    src/plaincraft/core/events/event_types.hpp
    src/plaincraft/core/events/types/input_event.cpp
    src/plaincraft/core/events/types/input_event.hpp
    src/plaincraft/core/events/types/loop_event.cpp
    src/plaincraft/core/events/types/loop_event.hpp
    src/plaincraft/core/models/models_cache.cpp
    src/plaincraft/core/models/models_cache.hpp
    src/plaincraft/core/input/input_manager.cpp
    src/plaincraft/core/input/input_manager.hpp
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
