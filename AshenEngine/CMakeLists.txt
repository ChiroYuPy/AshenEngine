project(AshenEngine)

# ===== VENDOR LIBRARIES =====

# Vendor sources
add_library(AshenVendor STATIC
        vendor/src/glad.c
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp
        vendor/src/tiny_obj_loader.cc
        vendor/src/miniaudio.cpp
)

target_include_directories(AshenVendor PUBLIC
        vendor/include
)

# ===== ENGINE LIBRARY =====

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "include/*.hpp")

add_library(AshenEngineLib STATIC
        ${ENGINE_SOURCES}
        ${ENGINE_HEADERS}
        include/Ashen/Nodes/CanvasItem.h
        src/Nodes/CanvasItem.cpp
        include/Ashen/Nodes/Node2D.h
        src/Nodes/Node2D.cpp
        include/Ashen/Nodes/Control.h
        include/Ashen/Nodes/Containers.h
        include/Ashen/Nodes/Widgets.h
        include/Ashen/Nodes/Node3D.h
        src/Nodes/Control.cpp
        src/Nodes/Containers.cpp
        src/Nodes/Widgets.cpp
        src/Nodes/Node3D.cpp
)

add_library(Ashen::Engine ALIAS AshenEngineLib)

target_include_directories(AshenEngineLib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AshenEngineLib
        PUBLIC
        OpenGL::GL
        glfw
        glm::glm
        AshenVendor
)

set_target_properties(AshenEngineLib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(AshenEngineLib PUBLIC
        GLFW_INCLUDE_NONE
)
