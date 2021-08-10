set(TARGET_NAME "Runner")
add_executable(${TARGET_NAME} "src/runner/main.cpp")

if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${TARGET_NAME} PRIVATE "-Wall -Wextra")
endif()
if ( MSVC )
    target_compile_options(${TARGET_NAME} PRIVATE "/W4")
endif()

target_link_libraries(${TARGET_NAME} PRIVATE "Core" "RenderEngine_Vulkan" "RenderEngine_OpenGL")
