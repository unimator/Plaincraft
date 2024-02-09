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
    src/plaincraft/common/debugging/logging/logger.cpp
    src/plaincraft/common/debugging/profiling/profiler.cpp
    src/plaincraft/common/utils/file_utils.cpp
)

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${TARGET_NAME} PUBLIC "libglew_static" "glfw" "glm")