# Glad
add_subdirectory(glad)

# ImGui
add_subdirectory(imgui)

# STB Image
add_subdirectory(stb_image)

# OpenSimplex2S
add_subdirectory(opensimplex2s)

# Tiny Obj Loader
add_subdirectory(tiny_obj_loader)

# Miniaudio
add_subdirectory(miniaudio)

# JSON (header-only)
add_subdirectory(json)

# Export all vendor libs as one interface
add_library(AshenVendor INTERFACE)

# IMPORTANT: Expose ALL vendor include directories for CLion
target_include_directories(AshenVendor INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb_image>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/opensimplex2s>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/miniaudio>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/json>
)

target_link_libraries(AshenVendor INTERFACE
        glad
        imgui
        stb_image
        opensimplex2s
        tiny_obj_loader
        miniaudio
        json
)