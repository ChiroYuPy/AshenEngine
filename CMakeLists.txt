cmake_minimum_required(VERSION 3.25)
project(Voxelity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(AshenEngine)
add_subdirectory(Voxelity)
add_subdirectory(Testbed)