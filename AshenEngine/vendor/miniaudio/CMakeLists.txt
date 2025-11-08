add_library(miniaudio STATIC miniaudio.cpp)

target_include_directories(miniaudio
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(miniaudio PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
