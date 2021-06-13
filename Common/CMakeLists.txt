add_library("Common" "")

target_sources("Common"
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

target_include_directories("Common" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries("Common" PUBLIC "libglew_static" "glfw" "glm")