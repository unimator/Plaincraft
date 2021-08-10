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
    include/plaincraft_common.hpp
    src/plaincraft/common/common.hpp
    src/plaincraft/common/exceptions/not_implemented_exception.hpp
    src/plaincraft/common/system_types.hpp
    src/plaincraft/common/system_types_glm.hpp
    src/plaincraft/common/utils/file_utils.cpp
    src/plaincraft/common/utils/file_utils.hpp
)

target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(${TARGET_NAME} PUBLIC "libglew_static" "glfw" "glm")