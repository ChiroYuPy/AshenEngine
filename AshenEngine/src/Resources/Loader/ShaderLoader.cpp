#include "Ashen/Resources/Loader/ShaderLoader.h"

#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/Utils/FileSystem.h"

namespace ash {
    ShaderProgram ShaderLoader::Load(
        const fs::path &vertPath,
        const fs::path &fragPath
    ) {
        if (!FileSystem::Exists(vertPath)) {
            throw std::runtime_error("Vertex shader not found: " + vertPath.string());
        }
        if (!FileSystem::Exists(fragPath)) {
            throw std::runtime_error("Fragment shader not found: " + fragPath.string());
        }

        // Read shader source files
        const String vertSource = FileSystem::ReadFileAsString(vertPath);
        const String fragSource = FileSystem::ReadFileAsString(fragPath);

        return FromSources(vertSource, fragSource);
    }

    ShaderProgram ShaderLoader::LoadWithGeometry(
        const fs::path &vertPath,
        const fs::path &fragPath,
        const fs::path &geomPath
    ) {
        if (!FileSystem::Exists(vertPath)) {
            throw std::runtime_error("Vertex shader not found: " + vertPath.string());
        }
        if (!FileSystem::Exists(fragPath)) {
            throw std::runtime_error("Fragment shader not found: " + fragPath.string());
        }
        if (!FileSystem::Exists(geomPath)) {
            throw std::runtime_error("Geometry shader not found: " + geomPath.string());
        }

        // Read shader source files
        const String vertSource = FileSystem::ReadFileAsString(vertPath);
        const String fragSource = FileSystem::ReadFileAsString(fragPath);
        const String geomSource = FileSystem::ReadFileAsString(geomPath);

        // Create and compile shader units
        ShaderUnit vertexShader(ShaderStage::Vertex, vertSource);
        ShaderUnit fragmentShader(ShaderStage::Fragment, fragSource);
        ShaderUnit geometryShader(ShaderStage::Geometry, geomSource);

        // Create program and link
        ShaderProgram program;
        program.AttachShader(std::move(vertexShader));
        program.AttachShader(std::move(fragmentShader));
        program.AttachShader(std::move(geometryShader));
        program.Link();

        return program;
    }

    ShaderProgram ShaderLoader::FromSources(
        const String &vertSource,
        const String &fragSource
    ) {
        // Create and compile shader units
        ShaderUnit vertexShader(ShaderStage::Vertex, vertSource);
        ShaderUnit fragmentShader(ShaderStage::Fragment, fragSource);

        // Create program and link
        ShaderProgram program;
        program.AttachShader(std::move(vertexShader));
        program.AttachShader(std::move(fragmentShader));
        program.Link();

        return program;
    }
}