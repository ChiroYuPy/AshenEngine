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
        src/voxelWorld/world/TextureColorPalette.cpp
        src/voxelWorld/world/VoxelRaycaster.cpp
        src/voxelWorld/world/World.cpp
        src/voxelWorld/world/WorldInteractor.cpp
        src/voxelWorld/world/WorldRenderer.cpp
        src/layers/VoxelWorldLayer.cpp
        src/entities/Player.cpp
        include/Voxelity/entities/Entity.h
        include/Voxelity/systems/PhysicsSystem.h
        src/systems/PhysicsSystem.cpp
        include/Voxelity/player/PlayerController.h
        src/player/PlayerController.cpp
        src/input/InputHandler.cpp
        include/Voxelity/input/InputHandler.h
        src/entities/EntityManager.cpp
        include/Voxelity/entities/EntityManager.h
        include/Voxelity/entities/FallingBlockEntity.h
        src/entities/FallingBlockEntity.cpp
        src/layers/UILayer.cpp
        include/Voxelity/layers/UILayer.h
        src/layers/DebugLayer.cpp
        include/Voxelity/layers/DebugLayer.h
)

add_executable(Voxelity ${APP_SOURCES})

target_include_directories(Voxelity
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Voxelity PRIVATE Core)

add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(Voxelity copy_resources)