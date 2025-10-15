project(AshenEditor)

# ===== IMGUI LIBRARY =====
# Pas besoin de find_package ici non plus

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

add_library(ImGui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ImGui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(ImGui PUBLIC
        glfw
        OpenGL::GL
)

target_compile_definitions(ImGui PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# ===== EDITOR EXECUTABLE =====

file(GLOB_RECURSE EDITOR_SOURCES "src/*.cpp")

add_executable(AshenEditor
        ${EDITOR_SOURCES}
)

target_link_libraries(AshenEditor
        PRIVATE
        Ashen::Engine
        ImGui
)

set_target_properties(AshenEditor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(run_editor
        COMMAND AshenEditor
        DEPENDS AshenEditor
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
)
