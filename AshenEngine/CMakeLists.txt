project(Ashen LANGUAGES CXX)

set(CORE_SOURCES
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp

        src/Core/Application.cpp
        src/Core/Input.cpp
        src/Core/LayerStack.cpp
        src/Core/Platform.cpp
        src/Core/Window.cpp

        src/GraphicsAPI/RenderCommand.cpp
        src/Graphics/Rendering/Renderer.cpp
        src/Graphics/Rendering/Renderer2D.cpp

        src/Utils/FileSystem.cpp
        src/Utils/ImageLoader.cpp
        src/Resources/ResourceManager.cpp
        src/Graphics/Rendering/Renderer3D.cpp
        src/Resources/Loader/ShaderLoader.cpp
        src/Resources/Loader/TextureLoader.cpp
        include/Ashen/GraphicsAPI/GraphicsContext.h
        src/GraphicsAPI/GraphicsContext.cpp

)

add_library(Ashen STATIC ${CORE_SOURCES})

# Headers publics
target_include_directories(Ashen
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/include
)

# Ajoute glad
add_subdirectory(vendor/glad)

find_package(glfw3 REQUIRED)


target_link_libraries(Ashen PUBLIC glad glfw)
