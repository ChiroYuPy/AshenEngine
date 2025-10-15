cmake_minimum_required(VERSION 3.18)
project(Ashen VERSION 1.0 LANGUAGES CXX)

option(BUILD_EDITOR "Build the editor" ON)
option(BUILD_TESTBED "Build the testbed" ON)
option(BUILD_VOXELITY "Build voxelity" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===== TROUVER TOUTES LES DÉPENDANCES ICI =====
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Ajouter les sous-projets
add_subdirectory(AshenEngine)

if (BUILD_EDITOR)
    add_subdirectory(Editor)
endif ()

if (BUILD_TESTBED)
    add_subdirectory(Testbed)
endif ()

if (BUILD_VOXELITY)
    add_subdirectory(Voxelity)
endif ()
