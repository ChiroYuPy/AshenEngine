project(Ashen LANGUAGES CXX)

# ===========================
# Core sources
# ===========================
set(CORE_SOURCES
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp

        src/Core/Application.cpp
        src/Core/Input.cpp
        src/Core/LayerStack.cpp
        src/Core/Platform.cpp
        src/Core/Window.cpp

        src/GraphicsAPI/RenderContext.cpp
        src/GraphicsAPI/GraphicsContext.cpp

        src/Graphics/Rendering/Renderer.cpp
        src/Graphics/Rendering/Renderer2D.cpp
        src/Graphics/Rendering/Renderer3D.cpp
        src/Graphics/Objects/Material.cpp
        src/Graphics/Objects/Mesh.cpp

        src/Utils/FileSystem.cpp
        src/Utils/ImageLoader.cpp

        src/Resources/ResourceManager.cpp
        src/Resources/Loader/ShaderLoader.cpp
        src/Resources/Loader/TextureLoader.cpp
        src/Resources/Loader/MeshLoader.cpp

        src/BuiltIn/BuiltInShader.cpp
)

# ===========================
# TinyObjLoader
# ===========================
set(TINYOBJ_SOURCES
        vendor/tinyobjloader/tiny_obj_loader.cc
        include/Ashen/Graphics/Objects/Light.h
)

# ===========================
# Add library
# ===========================
add_library(Ashen STATIC
        ${CORE_SOURCES}
        ${TINYOBJ_SOURCES}
)

# ===========================
# Include directories
# ===========================
target_include_directories(Ashen
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/tinyobjloader
)

# ===========================
# External dependencies
# ===========================
add_subdirectory(vendor/glad)

find_package(glfw3 REQUIRED)

target_link_libraries(Ashen PUBLIC glad glfw)