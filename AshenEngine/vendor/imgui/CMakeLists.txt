add_library(imgui STATIC
        imgui.cpp
        imgui_draw.cpp
        imgui_demo.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/backends>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(imgui
        PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)

# ImGui depends on GLFW, Glad
target_link_libraries(imgui
        PUBLIC
        glad
        glfw
)

set_target_properties(imgui PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
