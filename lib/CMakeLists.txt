cmake_minimum_required(VERSION 3.20.0)

if (CONFIG_STRATEGY_UI)
add_subdirectory(ui)
endif()

if (CONFIG_STRATEGY_GAME)
add_subdirectory(game)
endif()

zephyr_include_directories(.)
