project(AshenTestbed)

file(GLOB_RECURSE TESTBED_SOURCES "src/*.cpp")
file(GLOB_RECURSE TESTBED_HEADERS "include/*.h")

add_executable(AshenTestbed
        ${TESTBED_SOURCES}
        ${TESTBED_HEADERS}
        include/layers/GameLayer.h
        src/layers/GameLayer.cpp
)

target_include_directories(AshenTestbed PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(AshenTestbed
        PRIVATE
        Ashen::Engine
)

set_target_properties(AshenTestbed PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_target(run_testbed
        COMMAND AshenTestbed
        DEPENDS AshenTestbed
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Testbed
        USES_TERMINAL
)