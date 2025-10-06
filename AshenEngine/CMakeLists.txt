project(Ashen LANGUAGES CXX)

set(CORE_SOURCES
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp

        src/Core/Application.cpp
        src/Core/Input.cpp
        src/Core/LayerStack.cpp
        src/Core/Platform.cpp
        src/Core/Window.cpp

        src/GraphicsAPI/GLUtils.cpp
        src/GraphicsAPI/RenderCommand.cpp
        src/Graphics/Rendering/Renderer.cpp
        src/Graphics/Rendering/Renderer2D.cpp

        src/Utils/FileSystem.cpp
        src/Utils/ImageLoader.cpp
        src/Resources/ResourceManager.cpp
        src/Graphics/Rendering/Renderer3D.cpp
        include/Ashen/Config/IConfig.h
        include/Ashen/Config/JsonConfig.h
        include/Ashen/Core/UID.h

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
