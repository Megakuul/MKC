cmake_minimum_required(VERSION 3.17)

project(uritopath C)

add_library(uritopath uritopath.c)
target_include_directories(uritopath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})