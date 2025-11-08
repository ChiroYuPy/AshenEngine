add_library(opensimplex2s STATIC OpenSimplex2S.cpp)

target_include_directories(opensimplex2s
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(opensimplex2s PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
