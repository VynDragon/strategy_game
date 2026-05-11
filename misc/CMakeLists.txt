cmake_minimum_required(VERSION 3.20.0)

zephyr_include_directories(.)

target_sources(app PRIVATE
ui_style.c
)
