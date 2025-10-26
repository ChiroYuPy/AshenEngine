project(AshenEngine)

# ===== VENDOR LIBRARIES =====

# Vendor sources
add_library(AshenVendor STATIC
        vendor/src/glad.c
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp
        vendor/src/tiny_obj_loader.cc
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
        include/Ashen/Math/Random.h
        src/Scene/Node.cpp
        include/Ashen/Graphics/UI/UIWidgets.h
        include/Ashen/Graphics/UI/UIManager.h
        src/Graphics/UI/UIWidgets.cpp
        include/Ashen/Scene/Control.h
        include/Ashen/Scene/SceneTree.h
        include/Ashen/Graphics/UI/UIServer.h
        include/Ashen/Graphics/UI/UIContainers.h
        src/Scene/Control.cpp
        src/Graphics/UI/UIContainers.cpp
        src/Graphics/UI/UIServer.cpp
        src/Scene/SceneTree.cpp
        include/Ashen/Events/EventBus.h
        include/Ashen/GraphicsAPI/RenderState.h
        include/Ashen/Utils/Utilities.h
        include/Ashen/Core/Codes.h
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
        AshenVendor
)

set_target_properties(AshenEngineLib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(AshenEngineLib PUBLIC
        GLFW_INCLUDE_NONE
)
