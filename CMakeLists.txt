cmake_minimum_required(VERSION 3.10)
project(ZPZ-RGR)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
endif()

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

include_directories("/usr/include/jsoncpp")
target_link_libraries(prog.elf jsoncpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.16.0/googletest-1.16.0.tar.gz
  
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(math_unit_tests tests/test_math_func.cpp)
target_link_libraries(math_unit_tests gtest gtest_main jsoncpp)
add_test(NAME math_unit_tests COMMAND math_unit_tests)