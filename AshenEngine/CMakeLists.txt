project(Core LANGUAGES CXX)

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

        include/Ashen/math/AABB.h
        include/Ashen/math/Math.h
        include/Ashen/resources/ResourceSystem.h

)

add_library(Core STATIC ${CORE_SOURCES})

# Headers publics
target_include_directories(Core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/include
)

# Ajoute glad
add_subdirectory(vendor/glad)

find_package(glfw3 REQUIRED)


target_link_libraries(Core PUBLIC glad glfw)
