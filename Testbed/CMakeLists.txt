project(AshenTestbed)

# Recherche récursive des fichiers sources et headers
file(GLOB_RECURSE TESTBED_SOURCES "src/*.cpp")
file(GLOB_RECURSE TESTBED_HEADERS "include/*.h")

# Création de l'exécutable
add_executable(AshenTestbed
        ${TESTBED_SOURCES}
        ${TESTBED_HEADERS}
)

# Inclusion des headers
target_include_directories(AshenTestbed PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Linker avec la bibliothèque engine
target_link_libraries(AshenTestbed
        PRIVATE
        Ashen::Engine
)

# Options de build
set_target_properties(AshenTestbed PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Testbed"
)

# Copie récursive et automatique de tout le dossier resources dans le dossier du binaire
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/bin/resources
        COMMENT "Copying resources to build directory..."
)

# S'assurer que la copie des resources est faite avant que AshenTestbed soit construit
add_dependencies(AshenTestbed copy_resources)

# Cible pour lancer le testbed
add_custom_target(run_testbed
        COMMAND AshenTestbed
        DEPENDS AshenTestbed
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Testbed
        USES_TERMINAL
)
