set(TARGET_NAME "Assets")

add_custom_target(${TARGET_NAME} ALL)

function(copy_assets assets_directory)
        add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/${assets_directory} ${CMAKE_BINARY_DIR}/${BINARY_OUTPUT}/${TARGET_NAME}/${assets_directory})
endfunction()

copy_assets(Models)
copy_assets(Textures)