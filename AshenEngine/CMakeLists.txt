project(AshenEngine)

# ===== VENDOR LIBRARIES =====
add_subdirectory(vendor)

# ===== ENGINE LIBRARY =====
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "include/*.h" "include/*.hpp")

add_library(AshenEngineLib STATIC
        ${ENGINE_SOURCES}
        ${ENGINE_HEADERS}
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
        AshenVendor
)

target_compile_definitions(AshenEngineLib PUBLIC
        GLFW_INCLUDE_NONE
)

# Enable profiling in Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(AshenEngineLib PUBLIC ASHEN_ENABLE_PROFILING)
endif()

set_target_properties(AshenEngineLib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)