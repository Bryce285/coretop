cmake_minimum_required(VERSION 3.10)
project(system_monitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(external/FTXUI)

file(GLOB SOURCES "src/*.cpp")
add_executable(system_monitor ${SOURCES})

target_link_libraries(system_monitor
  PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)
