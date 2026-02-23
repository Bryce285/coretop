cmake_minimum_required(VERSION 3.10)
project(coretop VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(external/FTXUI)

file(GLOB SOURCES "src/*.cpp")
add_executable(coretop ${SOURCES})

target_compile_options(coretop PRIVATE
	-Wvla
	-fstack-protector-strong
	-Wall
	-Wextra
	-Wpedantic
	-Wshadow
	-Wconversion
)

target_link_libraries(coretop
  PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)
