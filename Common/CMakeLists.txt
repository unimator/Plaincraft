set(TARGET_NAME "Common")
add_library(${TARGET_NAME} "")

if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${TARGET_NAME} PRIVATE "-Wall -Wextra")
endif()
if ( MSVC )
    target_compile_options(${TARGET_NAME} PRIVATE "/W4")
endif()

target_sources(${TARGET_NAME}
PRIVATE
    include/lib/stb_image.h
    include/lib/tiny_obj_loader.h
    include/plaincraft_common.hpp
    src/plaincraft/common/events/event_listener.hpp
    src/plaincraft/common/events/event_publisher.hpp
    src/plaincraft/common/events/event_subscription_base.hpp
    src/plaincraft/common/events/event_subscription.hpp
    src/plaincraft/common/events/event_trigger_base.hpp
    src/plaincraft/common/events/event_trigger.hpp
    src/plaincraft/common/exceptions/not_implemented_exception.hpp
    src/plaincraft/common/profiling/profiler.cpp
    src/plaincraft/common/profiling/profiler.hpp
    src/plaincraft/common/utils/file_utils.cpp
    src/plaincraft/common/utils/file_utils.hpp
    src/plaincraft/common/utils/fixed_list.hpp
    src/plaincraft/common/utils/hash_utils.hpp
    src/plaincraft/common/common.hpp
    src/plaincraft/common/system_types.hpp
    src/plaincraft/common/system_types_glm.hpp
)

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${TARGET_NAME} PUBLIC "libglew_static" "glfw" "glm")