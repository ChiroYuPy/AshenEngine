project(AshenEngine)

# ===== VENDOR LIBRARIES =====

# GLAD (OpenGL loader)
add_subdirectory(vendor/glad)

# Vendor sources
add_library(AshenVendor STATIC
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp
        vendor/tinyobjloader/tiny_obj_loader.cc
)

target_include_directories(AshenVendor PUBLIC
        vendor/include
        vendor/tinyobjloader
)

# ===== ENGINE LIBRARY =====

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "include/*.hpp")

add_library(AshenEngineLib STATIC
        ${ENGINE_SOURCES}
        ${ENGINE_HEADERS}
        include/Ashen/Scene/Node.h
        include/Ashen/Math/Transform.h
        include/Ashen/Scene/SceneTree.h
        include/Ashen/Scene/2D/Control.h
        include/Ashen/Scene/2D/Container.h
        src/Scene/Node.cpp
)

add_library(Ashen::Engine ALIAS AshenEngineLib)

target_include_directories(AshenEngineLib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Pas besoin de find_package ici car déjà fait dans le root
target_link_libraries(AshenEngineLib
        PUBLIC
        OpenGL::GL
        glfw
        glm::glm
        glad
        AshenVendor
)

set_target_properties(AshenEngineLib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(AshenEngineLib PUBLIC
        GLFW_INCLUDE_NONE
)
