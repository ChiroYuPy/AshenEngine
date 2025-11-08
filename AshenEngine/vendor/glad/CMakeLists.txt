add_library(glad STATIC glad.c)

target_include_directories(glad
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Link against OpenGL if available, else it's just loader
if(OPENGL_FOUND)
    target_link_libraries(glad PUBLIC OpenGL::GL)
endif()
