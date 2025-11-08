add_library(tiny_obj_loader STATIC tiny_obj_loader.cc)

target_include_directories(tiny_obj_loader
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(tiny_obj_loader PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
