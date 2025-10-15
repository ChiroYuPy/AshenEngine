#include "Ashen/Resources/Loader/ShaderLoader.h"

namespace ash {
    ShaderProgram ShaderLoader::Load(const fs::path& vertPath, const fs::path& fragPath) {
        if (!FileSystem::Exists(vertPath))
            throw std::runtime_error("Vertex shader not found: " + vertPath.string());
        if (!FileSystem::Exists(fragPath))
            throw std::runtime_error("Fragment shader not found: " + fragPath.string());

        const std::string vertSource = FileSystem::ReadFileAsString(vertPath);
        const std::string fragSource = FileSystem::ReadFileAsString(fragPath);

        return FromSources(vertSource, fragSource);
    }

    ShaderProgram ShaderLoader::LoadWithGeometry(
        const fs::path& vertPath,
        const fs::path& fragPath,
        const fs::path& geomPath
    ) {
        if (!FileSystem::Exists(vertPath))
            throw std::runtime_error("Vertex shader not found: " + vertPath.string());
        if (!FileSystem::Exists(fragPath))
            throw std::runtime_error("Fragment shader not found: " + fragPath.string());
        if (!FileSystem::Exists(geomPath))
            throw std::runtime_error("Geometry shader not found: " + geomPath.string());

        const std::string vertSource = FileSystem::ReadFileAsString(vertPath);
        const std::string fragSource = FileSystem::ReadFileAsString(fragPath);
        const std::string geomSource = FileSystem::ReadFileAsString(geomPath);

        ShaderProgram program;
        
        const ShaderUnit vertex(ShaderStage::Vertex, vertSource);
        const ShaderUnit fragment(ShaderStage::Fragment, fragSource);
        const ShaderUnit geometry(ShaderStage::Geometry, geomSource);

        program.AttachShader(vertex);
        program.AttachShader(fragment);
        program.AttachShader(geometry);
        program.Link();

        return program;
    }

    ShaderProgram ShaderLoader::FromSources(
        const std::string& vertSource,
        const std::string& fragSource
    ) {
        ShaderProgram program;

        const ShaderUnit vertex(ShaderStage::Vertex, vertSource);
        const ShaderUnit fragment(ShaderStage::Fragment, fragSource);

        program.AttachShader(vertex);
        program.AttachShader(fragment);
        program.Link();

        return program;
    }

    bool ShaderLoader::ShaderExists(const fs::path& basePath, const std::string& shaderName) {
        const fs::path vertPath = basePath / (shaderName + ".vert");
        const fs::path fragPath = basePath / (shaderName + ".frag");
        
        return FileSystem::Exists(vertPath) && FileSystem::Exists(fragPath);
    }

    Vector<std::string> ShaderLoader::ScanForShaders(const fs::path& directory) {
        Vector<std::string> shaders;
        const auto files = FileSystem::ScanDirectory(directory, {".vert"}, true);

        for (const auto& file : files) {
            std::string name = file.stem().string();
            auto fragPath = file.parent_path() / (name + ".frag");
            
            if (FileSystem::Exists(fragPath)) {
                shaders.push_back(name);
            }
        }

        return shaders;
    }
}