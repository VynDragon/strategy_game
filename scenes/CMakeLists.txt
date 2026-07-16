cmake_minimum_required(VERSION 3.20.0)

if (CONFIG_TEASER_SCENES)
add_subdirectory(teaser)
endif()

zephyr_include_directories(.)
zephyr_include_directories(skyboxes)
