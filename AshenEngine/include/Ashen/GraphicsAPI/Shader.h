#ifndef ASHEN_SHADER_H
#define ASHEN_SHADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <filesystem>

#include <glad/glad.h>

#include "Ashen/Math/Math.h"
#include "Ashen/GraphicsAPI/GLEnums.h"
#include "Ashen/GraphicsAPI/GLObject.h"

namespace ash {
    class ShaderUnit final : public GLObject {
    public:
        ShaderUnit(const ShaderStage stage, const std::string &source)
            : m_Stage(stage) {
            Compile(source);
        }

        ~ShaderUnit() override {
            if (m_ID)
                glDeleteShader(m_ID);
        }

        ShaderUnit(const ShaderUnit &) = delete;

        ShaderUnit &operator=(const ShaderUnit &) = delete;

        [[nodiscard]] ShaderStage Stage() const { return m_Stage; }

        static ShaderUnit FromFile(ShaderStage stage, const std::string &filepath) {
            std::ifstream file(filepath);
            if (!file.is_open())
                throw std::runtime_error("Failed to open shader file: " + filepath);

            std::stringstream buffer;
            buffer << file.rdbuf();
            return ShaderUnit(stage, buffer.str());
        }

    private:
        ShaderStage m_Stage;

        void Compile(const std::string &source) {
            m_ID = glCreateShader(static_cast<GLenum>(m_Stage));
            const char *src = source.c_str();
            glShaderSource(m_ID, 1, &src, nullptr);
            glCompileShader(m_ID);

            GLint success;
            glGetShaderiv(m_ID, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLint length;
                glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length);
                std::string infoLog(length, ' ');
                glGetShaderInfoLog(m_ID, length, &length, infoLog.data());
                glDeleteShader(m_ID);
                m_ID = 0;

                const std::string stageStr = GetStageName(m_Stage);
                throw std::runtime_error(stageStr + " shader compilation failed:\n" + infoLog);
            }
        }

        static std::string GetStageName(const ShaderStage stage) {
            switch (stage) {
                case ShaderStage::Vertex: return "Vertex";
                case ShaderStage::Fragment: return "Fragment";
                case ShaderStage::Geometry: return "Geometry";
                case ShaderStage::TessControl: return "Tessellation Control";
                case ShaderStage::TessEvaluation: return "Tessellation Evaluation";
                case ShaderStage::Compute: return "Compute";
                default: return "Unknown";
            }
        }
    };

    struct ShaderConfig {
        bool validateOnLink = true;
        bool detachAfterLink = true;
        bool throwOnWarning = false;
        bool cacheUniforms = true;
        bool warnOnMissingUniform = true;

        static ShaderConfig Default() {
            return {};
        }

        static ShaderConfig Release() {
            ShaderConfig config;
            config.validateOnLink = false;
            config.warnOnMissingUniform = false;
            return config;
        }

        static ShaderConfig Debug() {
            ShaderConfig config;
            config.validateOnLink = true;
            config.throwOnWarning = true;
            config.warnOnMissingUniform = true;
            return config;
        }
    };

    class ShaderProgram final : public Bindable {
    public:
        explicit ShaderProgram(const ShaderConfig &config = ShaderConfig::Default())
            : m_Config(config) {
        }

        ~ShaderProgram() override {
            if (m_ID)
                glDeleteProgram(m_ID);
        }

        ShaderProgram(const ShaderProgram &) = delete;

        ShaderProgram &operator=(const ShaderProgram &) = delete;

        ShaderProgram(ShaderProgram &&other) noexcept
            : m_UniformCache(std::move(other.m_UniformCache)),
              m_AttachedStages(std::move(other.m_AttachedStages)),
              m_AttachedShaderIDs(std::move(other.m_AttachedShaderIDs)),
              m_WarnedUniforms(std::move(other.m_WarnedUniforms)),
              m_Config(other.m_Config) {
            m_ID = other.m_ID;
            other.m_ID = 0;
        }

        ShaderProgram &operator=(ShaderProgram &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteProgram(m_ID);
                m_ID = other.m_ID;
                m_UniformCache = std::move(other.m_UniformCache);
                m_AttachedStages = std::move(other.m_AttachedStages);
                m_AttachedShaderIDs = std::move(other.m_AttachedShaderIDs);
                m_WarnedUniforms = std::move(other.m_WarnedUniforms);
                m_Config = other.m_Config;
                other.m_ID = 0;
            }
            return *this;
        }

        void Bind() const override {
            glUseProgram(m_ID);
        }

        void Unbind() const override {
            glUseProgram(0);
        }

        void AttachShader(const ShaderUnit &shader) {
            const ShaderStage stage = shader.Stage();
            if (m_AttachedStages.contains(stage))
                throw std::runtime_error("ShaderProgram already has a shader of this stage attached!");

            if (!m_ID) m_ID = glCreateProgram();
            glAttachShader(m_ID, shader.ID());
            m_AttachedStages.insert(stage);
            m_AttachedShaderIDs.push_back(shader.ID());
        }

        void Link() {
            if (!m_ID)
                throw std::runtime_error("No shaders attached to program!");

            glLinkProgram(m_ID);

            GLint success;
            glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
            if (!success) {
                GLint length;
                glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);
                std::string infoLog(length, ' ');
                glGetProgramInfoLog(m_ID, length, &length, infoLog.data());
                glDeleteProgram(m_ID);
                m_ID = 0;
                throw std::runtime_error("ShaderProgram linking failed:\n" + infoLog);
            }

            if (m_Config.validateOnLink) {
                Validate();
            }

            if (m_Config.detachAfterLink) {
                for (const GLuint id: m_AttachedShaderIDs)
                    glDetachShader(m_ID, id);
                m_AttachedShaderIDs.clear();
            }

            if (m_Config.cacheUniforms) {
                CacheAllUniforms();
            }
        }

        void Validate() const {
            glValidateProgram(m_ID);
            GLint success;
            glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &success);
            if (!success) {
                GLint length;
                glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);
                std::string infoLog(length, ' ');
                glGetProgramInfoLog(m_ID, length, &length, infoLog.data());

                if (m_Config.throwOnWarning)
                    throw std::runtime_error("ShaderProgram validation failed:\n" + infoLog);
                else
                    std::cerr << "ShaderProgram validation warning:\n" << infoLog << std::endl;
            }
        }

        // Uniform setters
        void SetBool(const std::string &name, bool value) const {
            glUniform1i(GetUniformLocation(name), static_cast<int>(value));
        }

        void SetInt(const std::string &name, int value) const {
            glUniform1i(GetUniformLocation(name), value);
        }

        void SetFloat(const std::string &name, float value) const {
            glUniform1f(GetUniformLocation(name), value);
        }

        void SetVec2(const std::string &name, const Vec2 &v) const {
            glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(v));
        }

        void SetVec2(const std::string &name, float x, float y) const {
            glUniform2f(GetUniformLocation(name), x, y);
        }

        void SetVec3(const std::string &name, const Vec3 &v) const {
            glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(v));
        }

        void SetVec3(const std::string &name, float x, float y, float z) const {
            glUniform3f(GetUniformLocation(name), x, y, z);
        }

        void SetVec4(const std::string &name, const Vec4 &v) const {
            glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(v));
        }

        void SetVec4(const std::string &name, float x, float y, float z, float w) const {
            glUniform4f(GetUniformLocation(name), x, y, z, w);
        }

        void SetMat3(const std::string &name, const Mat3 &m) const {
            glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
        }

        void SetMat4(const std::string &name, const Mat4 &m) const {
            glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
        }

        // Uniform block binding
        void BindUniformBlock(const std::string &name, uint32_t bindingPoint) const {
            const GLuint blockIndex = glGetUniformBlockIndex(m_ID, name.c_str());
            if (blockIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(m_ID, blockIndex, bindingPoint);
            } else if (m_Config.warnOnMissingUniform) {
                std::cerr << "Warning: Uniform block '" << name << "' not found!\n";
            }
        }

        // Shader storage block binding
        void BindStorageBlock(const std::string &name, uint32_t bindingPoint) const {
            const GLuint blockIndex = glGetProgramResourceIndex(m_ID, GL_SHADER_STORAGE_BLOCK, name.c_str());
            if (blockIndex != GL_INVALID_INDEX) {
                glShaderStorageBlockBinding(m_ID, blockIndex, bindingPoint);
            } else if (m_Config.warnOnMissingUniform) {
                std::cerr << "Warning: Storage block '" << name << "' not found!\n";
            }
        }

        [[nodiscard]] const ShaderConfig &GetConfig() const { return m_Config; }

        [[nodiscard]] bool HasUniform(const std::string &name) const {
            return GetUniformLocation(name) != -1;
        }

        // Builders style SFML
        static ShaderProgram FromFiles(const std::string &vertexPath,
                                       const std::string &fragmentPath,
                                       const ShaderConfig &config = ShaderConfig::Default()) {
            ShaderProgram program(config);

            const ShaderUnit vertShader = ShaderUnit::FromFile(ShaderStage::Vertex, vertexPath);
            const ShaderUnit fragShader = ShaderUnit::FromFile(ShaderStage::Fragment, fragmentPath);

            program.AttachShader(vertShader);
            program.AttachShader(fragShader);
            program.Link();

            return program;
        }

        static ShaderProgram FromSources(const std::string &vertexSource,
                                         const std::string &fragmentSource,
                                         const ShaderConfig &config = ShaderConfig::Default()) {
            ShaderProgram program(config);

            const ShaderUnit vertShader(ShaderStage::Vertex, vertexSource);
            const ShaderUnit fragShader(ShaderStage::Fragment, fragmentSource);

            program.AttachShader(vertShader);
            program.AttachShader(fragShader);
            program.Link();

            return program;
        }

        static ShaderProgram FromFilesWithGeometry(const std::string &vertexPath,
                                                   const std::string &fragmentPath,
                                                   const std::string &geometryPath,
                                                   const ShaderConfig &config = ShaderConfig::Default()) {
            ShaderProgram program(config);

            const ShaderUnit vertShader = ShaderUnit::FromFile(ShaderStage::Vertex, vertexPath);
            const ShaderUnit fragShader = ShaderUnit::FromFile(ShaderStage::Fragment, fragmentPath);
            const ShaderUnit geomShader = ShaderUnit::FromFile(ShaderStage::Geometry, geometryPath);

            program.AttachShader(vertShader);
            program.AttachShader(fragShader);
            program.AttachShader(geomShader);
            program.Link();

            return program;
        }

    private:
        mutable std::unordered_map<std::string, GLint> m_UniformCache;
        std::unordered_set<ShaderStage> m_AttachedStages;
        std::vector<GLuint> m_AttachedShaderIDs;
        mutable std::unordered_set<std::string> m_WarnedUniforms;
        ShaderConfig m_Config;

        GLint GetUniformLocation(const std::string &name) const {
            if (m_Config.cacheUniforms && m_UniformCache.contains(name))
                return m_UniformCache[name];

            const GLint loc = glGetUniformLocation(m_ID, name.c_str());

            if (loc == -1 && m_Config.warnOnMissingUniform && !m_WarnedUniforms.contains(name)) {
                std::cerr << "Warning: uniform '" << name << "' doesn't exist!\n";
                m_WarnedUniforms.insert(name);
            }

            if (m_Config.cacheUniforms)
                m_UniformCache[name] = loc;

            return loc;
        }

        void CacheAllUniforms() const {
            GLint count;
            glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &count);

            for (GLint i = 0; i < count; ++i) {
                char name[256];
                GLsizei length;
                GLint size;
                GLenum type;
                glGetActiveUniform(m_ID, i, sizeof(name), &length, &size, &type, name);

                const GLint location = glGetUniformLocation(m_ID, name);
                if (location != -1)
                    m_UniformCache[std::string(name)] = location;
            }
        }
    };
}

#endif // ASHEN_SHADER_H
