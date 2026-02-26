cmake_minimum_required(VERSION 3.10)
project(coretop VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(STATIC_LINKING "Enable static linking" OFF)

add_subdirectory(external/FTXUI)

file(GLOB SOURCES "src/*.cpp")
add_executable(coretop ${SOURCES})

target_include_directories(coretop PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(STATIC_LINKING)
    message(STATUS "Building with static libstdc++ and libgcc")

    target_link_options(coretop PRIVATE
        -static-libstdc++
        -static-libgcc
    )
endif()

target_compile_options(coretop PRIVATE
	-Wvla
	-fstack-protector-strong
	-Wall
	-Wextra
	-Wpedantic
	-Wshadow
	-Wconversion
)

target_link_libraries(coretop PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

set_target_properties(coretop PROPERTIES
    OUTPUT_NAME "coretop-${CMAKE_SYSTEM_PROCESSOR}"
)
