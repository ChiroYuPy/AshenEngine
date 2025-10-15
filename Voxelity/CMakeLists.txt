project(Voxelity)

file(GLOB_RECURSE VOXELITY_SOURCES "src/*.cpp")
file(GLOB_RECURSE VOXELITY_HEADERS "include/*.h")

add_executable(Voxelity
        ${VOXELITY_SOURCES}
        ${VOXELITY_HEADERS}
)

target_include_directories(Voxelity PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Voxelity
        PRIVATE
        Ashen::Engine
)

set_target_properties(Voxelity PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Voxelity"
)

add_custom_target(run_voxelity
        COMMAND Voxelity
        DEPENDS Voxelity
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Voxelity
        USES_TERMINAL
)