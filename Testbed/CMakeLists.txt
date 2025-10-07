project(Testbed LANGUAGES CXX)

set(APP_SOURCES
        src/TestbedApp.cpp
)

add_executable(Testbed ${APP_SOURCES})

target_include_directories(Testbed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Testbed PRIVATE Ashen)

add_custom_target(testbed_copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(Testbed testbed_copy_resources)