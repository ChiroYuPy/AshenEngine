project(Voxelity LANGUAGES CXX)

set(APP_SOURCES
        src/VoxelityApp.cpp

        src/voxelWorld/chunk/Chunk.cpp
        src/voxelWorld/chunk/ChunkMesh.cpp

        src/voxelWorld/generation/FlatTerrainGenerator.cpp
        src/voxelWorld/generation/NaturalTerrainGenerator.cpp

        src/voxelWorld/utils/DirectionUtils.cpp

        src/voxelWorld/voxel/VoxelArray.cpp
        src/voxelWorld/voxel/VoxelType.cpp

        src/voxelWorld/render/TextureColorPalette.cpp
        src/voxelWorld/world/VoxelRaycaster.cpp
        src/voxelWorld/world/World.cpp
        src/voxelWorld/world/WorldInteractor.cpp
        src/voxelWorld/render/WorldRenderer.cpp
        src/voxelWorld/world/ChunkManager.cpp

        src/layers/VoxelWorldLayer.cpp
        src/layers/UILayer.cpp
        src/layers/DebugLayer.cpp

        src/systems/PhysicsSystem.cpp

        src/input/InputHandler.cpp

        src/entities/EntityManager.cpp
        src/entities/FallingBlockEntity.cpp
        src/entities/Player.cpp

        src/player/PlayerController.cpp
)

add_executable(Voxelity ${APP_SOURCES})

target_include_directories(Voxelity
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Voxelity PRIVATE Ashen)

add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(Voxelity copy_resources)