project(AshenEngine)

# ===== VENDOR LIBRARIES =====

# Vendor sources
add_library(AshenVendor STATIC
        vendor/src/glad.c
        vendor/src/IMG.cpp
        vendor/src/OpenSimplex2S.cpp
        vendor/src/tiny_obj_loader.cc
        vendor/src/miniaudio.cpp
        vendor/src/imgui.cpp
        vendor/src/imgui_draw.cpp
        vendor/src/imgui_tables.cpp
        vendor/src/imgui_widgets.cpp
        vendor/src/imgui_demo.cpp
        vendor/src/imgui_impl_glfw.cpp
        vendor/src/imgui_impl_opengl3.cpp
)

target_include_directories(AshenVendor PUBLIC
        vendor/include
        ${CMAKE_CURRENT_BINARY_DIR}/../vcpkg_installed/x64-windows/include
)

target_link_libraries(AshenVendor PUBLIC
        glfw
)

# ===== ENGINE LIBRARY =====

file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "include/*.hpp")

add_library(AshenEngineLib STATIC
        ${ENGINE_SOURCES}
        ${ENGINE_HEADERS}
        src/GraphicsAPI/RendererAPI.cpp
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
        nlohmann_json::nlohmann_json
        AshenVendor
)

set_target_properties(AshenEngineLib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(AshenEngineLib PUBLIC
        GLFW_INCLUDE_NONE
)

# Enable profiling in Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(AshenEngineLib PUBLIC ASHEN_ENABLE_PROFILING)
endif()
