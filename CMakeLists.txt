cmake_minimum_required(VERSION 3.10)
project(ZPZ-RGR)

set(CMAKE_CXX_STANDARD 17)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)

pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS} include)
file(GLOB SOURCES "src/*.cpp")

add_executable(prog.elf main.cpp ${SOURCES})

target_include_directories(prog.elf PRIVATE ${GTKMM_INCLUDE_DIRS})

target_link_libraries(prog.elf ${GTKMM_LIBRARIES})
