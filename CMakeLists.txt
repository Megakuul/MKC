cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 23)

project(xmdr)
file(GLOB_RECURSE SOURCE "src/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS} includes style)
link_directories(${GTKMM_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES})
