project(Ashen LANGUAGES CXX)

set(CORE_SOURCES
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp

        src/core/Application.cpp
        src/core/Input.cpp
        src/core/LayerStack.cpp
        src/core/Platform.cpp
        src/core/Window.cpp

        src/renderer/GLUtils.cpp
        src/renderer/RenderCommand.cpp
        src/renderer/Renderer.cpp
        src/renderer/Renderer2D.cpp

        include/Ashen/math/Math.h
        include/Ashen/utils/FileSystem.h
        src/utils/FileSystem.cpp
        include/Ashen/utils/ImageLoader.h
        src/utils/ImageLoader.cpp
        include/Ashen/resources/ResourceManager.h
        src/resources/ResourceManager.cpp
        include/Ashen/math/BoundingBox.h
        src/renderer/Renderer3D.cpp
        include/Ashen/renderer/Renderer3D.h
        include/Ashen/graphics/skybox/ISkybox.h
        include/Ashen/graphics/skybox/SkyboxCubeMap.h
        include/Ashen/renderer/gfx/GLEnums.h

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
