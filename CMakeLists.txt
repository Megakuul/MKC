cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 23)

project(mkc)

set(SOURCE
  src/main.cpp
  src/bridge.cpp
  src/fsutil.cpp
  src/keyhandler.cpp
  src/Components/Browser.cpp
  src/Components/Modal.cpp
  src/Components/Toolbar.cpp
  src/Components/RunShell.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

find_package(ZLIB REQUIRED)

add_subdirectory(external/uritopath)

include_directories(${GTKMM_INCLUDE_DIRS} includes)
link_directories(${GTKMM_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES} ZLIB::ZLIB uritopath)
