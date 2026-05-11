cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(scene_previewer)

target_sources(app PRIVATE src/main.c)

zephyr_include_directories(include)

add_subdirectory(scenes)
add_subdirectory(misc)

set(TOOLS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/../../../zephyr_3d_game_template2/tools)

set(SCENE_TO_PREVIEW ${CONFIG_SCENE_TO_PREVIEW})
zephyr_compile_definitions(
-DSCENE_TO_PREVIEW=${SCENE_TO_PREVIEW}
)

add_custom_command(
OUTPUT generated/generated_objects.h
COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_objects.py ${TOOLS_FOLDER} ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_DATA_PATH} generated
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(generate_objects DEPENDS generated/generated_objects.h)

zephyr_include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)

add_dependencies(app generate_objects)
