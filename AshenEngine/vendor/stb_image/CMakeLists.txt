add_library(stb_image STATIC stb_image_impl.cpp)

target_include_directories(stb_image
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(stb_image PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
