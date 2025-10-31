#ifndef ASHEN_SHADER_H
#define ASHEN_SHADER_H

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <glad/glad.h>

#include "Ashen/Math/Math.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Core/Types.h"
#include "Ashen/GraphicsAPI/Enums.h"
#include "Ashen/GraphicsAPI/GLObject.h"

namespace ash {
    class ShaderException final : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class ShaderUnit final : public GLObject {
    public:
        ShaderUnit(const ShaderStage stage, const String &source)
            : m_Stage(stage) {
            Compile(source);
        }

        ~ShaderUnit() override {
            if (m_ID)
                glDeleteShader(m_ID);
        }

        ShaderUnit(const ShaderUnit &) = delete;

        ShaderUnit &operator=(const ShaderUnit &) = delete;

        ShaderUnit(ShaderUnit &&other) noexcept
            : m_Stage(other.m_Stage) {
            m_ID = other.m_ID;
            other.m_ID = 0;
        }

        ShaderUnit &operator=(ShaderUnit &&other) noexcept {
            if (this != &other) {
                if (m_ID)
                    glDeleteShader(m_ID);
                m_ID = other.m_ID;
                m_Stage = other.m_Stage;
                other.m_ID = 0;
            }
            return *this;
        }

        [[nodiscard]] ShaderStage Stage() const { return m_Stage; }

        GLuint Release() noexcept {
            const GLuint id = m_ID;
            m_ID = 0;
            return id;
        }

        static ShaderUnit FromFile(ShaderStage stage, const String &filepath) {
            std::ifstream file(filepath);
            if (!file.is_open())
                throw ShaderException("Failed to open shader file: " + filepath);

            std::stringstream buffer;
            buffer << file.rdbuf();
            return {stage, buffer.str()};
        }

    private:
        ShaderStage m_Stage;

        void Compile(const String &source) {
            m_ID = glCreateShader(static_cast<GLenum>(m_Stage));
            if (!m_ID)
                throw ShaderException("glCreateShader failed");

            const char *src = source.c_str();
            glShaderSource(m_ID, 1, &src, nullptr);
            glCompileShader(m_ID);

            GLint success = GL_FALSE;
            glGetShaderiv(m_ID, GL_COMPILE_STATUS, &success);
            if (success != GL_TRUE) {
                GLint length = 0;
                glGetShaderiv(m_ID, GL_INFO_LOG_LENGTH, &length);

                String infoLog;
                if (length > 0) {
                    Vector<char> buf(static_cast<size_t>(length));
                    glGetShaderInfoLog(m_ID, length, nullptr, buf.data());
                    infoLog.assign(buf.begin(), buf.end());
                } else {
                    infoLog = "<no info log>";
                }

                glDeleteShader(m_ID);
                m_ID = 0;

                const String stageStr = GetStageName(m_Stage);
                throw ShaderException(stageStr + " shader compilation failed:\n" + infoLog);
            }
        }

        static String GetStageName(const ShaderStage stage) {
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
            if (!m_ID) {
                Logger::Error() << "Attempt to bind an invalid shader program!";
                return;
            }
            glUseProgram(m_ID);
        }

        void Unbind() const override {
            glUseProgram(0);
        }

        void AttachShader(ShaderUnit &&shader) {
            const ShaderStage stage = shader.Stage();
            if (m_AttachedStages.contains(stage))
                throw ShaderException("ShaderProgram already has a shader of this stage attached!");

            if (!m_ID) {
                m_ID = glCreateProgram();
                if (!m_ID)
                    throw ShaderException("glCreateProgram failed");
            }

            const GLuint id = shader.Release(); // on récupère l'ID et on empêche le shader de le supprimer
            if (id == 0)
                throw ShaderException("ShaderUnit has no valid GL id to attach.");

            glAttachShader(m_ID, id);
            m_AttachedStages.insert(stage);
            m_AttachedShaderIDs.push_back(id);
        }

        void Link() {
            if (!m_ID)
                throw ShaderException("No shaders attached to program!");

            glLinkProgram(m_ID);

            GLint success = GL_FALSE;
            glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
            if (success != GL_TRUE) {
                GLint length = 0;
                glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);

                String infoLog;
                if (length > 0) {
                    Vector<char> buf(static_cast<size_t>(length));
                    glGetProgramInfoLog(m_ID, length, nullptr, buf.data());
                    infoLog.assign(buf.begin(), buf.end());
                } else {
                    infoLog = "<no info log>";
                }

                glDeleteProgram(m_ID);
                m_ID = 0;
                throw ShaderException("ShaderProgram linking failed:\n" + infoLog);
            }

            if (m_Config.validateOnLink)
                Validate();

            if (m_Config.detachAfterLink) {
                for (const GLuint id: m_AttachedShaderIDs)
                    glDetachShader(m_ID, id);
                m_AttachedShaderIDs.clear();
            }

            if (m_Config.cacheUniforms)
                CacheAllUniforms();
        }

        void Validate() const {
            if (!m_ID) {
                Logger::Error() << "Attempt to validate an invalid shader program!";
                return;
            }

            glValidateProgram(m_ID);
            GLint success = GL_FALSE;
            glGetProgramiv(m_ID, GL_VALIDATE_STATUS, &success);
            if (success != GL_TRUE) {
                GLint length = 0;
                glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);

                String infoLog;
                if (length > 0) {
                    Vector<char> buf(static_cast<size_t>(length));
                    glGetProgramInfoLog(m_ID, length, nullptr, buf.data());
                    infoLog.assign(buf.begin(), buf.end());
                } else {
                    infoLog = "<no info log>";
                }

                if (m_Config.throwOnWarning)
                    throw ShaderException("ShaderProgram validation failed:\n" + infoLog);

                Logger::Error() << "ShaderProgram validation warning:\n" << infoLog;
            }
        }

        void SetBool(const String &name, const bool value) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform1i(loc, static_cast<int>(value));
        }

        void SetInt(const String &name, const int value) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform1i(loc, value);
        }

        void SetFloat(const String &name, const float value) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform1f(loc, value);
        }

        void SetVec2(const String &name, const Vec2 &v) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform2fv(loc, 1, glm::value_ptr(v));
        }

        void SetVec2(const String &name, const float x, const float y) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform2f(loc, x, y);
        }

        void SetVec3(const String &name, const Vec3 &v) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform3fv(loc, 1, glm::value_ptr(v));
        }

        void SetVec3(const String &name, const float x, const float y, const float z) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform3f(loc, x, y, z);
        }

        void SetVec4(const String &name, const Vec4 &v) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform4fv(loc, 1, glm::value_ptr(v));
        }

        void SetVec4(const String &name, const float x, const float y, const float z, const float w) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniform4f(loc, x, y, z, w);
        }

        void SetMat3(const String &name, const Mat3 &m) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m));
        }

        void SetMat4(const String &name, const Mat4 &m) const {
            const GLint loc = GetUniformLocation(name);
            if (loc == -1) return;
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
        }

        // Uniform block binding
        void BindUniformBlock(const String &name, const uint32_t bindingPoint) const {
            if (!m_ID) {
                Logger::Error() << "Attempt to bind a uniform block on invalid program!";
                return;
            }
            const GLuint blockIndex = glGetUniformBlockIndex(m_ID, name.c_str());
            if (blockIndex != GL_INVALID_INDEX) {
                glUniformBlockBinding(m_ID, blockIndex, bindingPoint);
            } else if (m_Config.warnOnMissingUniform) {
                Logger::Error() << "Warning: Uniform block '" << name << "' not found!";
            }
        }

        // Shader storage block binding
        void BindStorageBlock(const String &name, const uint32_t bindingPoint) const {
            if (!m_ID) {
                Logger::Error() << "Attempt to bind a storage block on invalid program!";
                return;
            }

            const GLuint blockIndex = glGetProgramResourceIndex(m_ID, GL_SHADER_STORAGE_BLOCK, name.c_str());
            if (blockIndex != GL_INVALID_INDEX) {
                glShaderStorageBlockBinding(m_ID, blockIndex, bindingPoint);
            } else if (m_Config.warnOnMissingUniform) {
                Logger::Error() << "Warning: Storage block '" << name << "' not found!";
            }
        }

        [[nodiscard]] const ShaderConfig &GetConfig() const { return m_Config; }

        [[nodiscard]] bool HasUniform(const String &name) const {
            return GetUniformLocation(name) != -1;
        }

        // Builders style SFML
        static ShaderProgram FromFiles(const String &vertexPath,
                                       const String &fragmentPath,
                                       const ShaderConfig &config = ShaderConfig::Default()) {
            ShaderProgram program(config);

            ShaderUnit vertShader = ShaderUnit::FromFile(ShaderStage::Vertex, vertexPath);
            ShaderUnit fragShader = ShaderUnit::FromFile(ShaderStage::Fragment, fragmentPath);

            program.AttachShader(std::move(vertShader));
            program.AttachShader(std::move(fragShader));
            program.Link();

            return program;
        }

        static ShaderProgram FromSources(const String &vertexSource,
                                         const String &fragmentSource,
                                         const ShaderConfig &config = ShaderConfig::Default()) {
            ShaderProgram program(config);

            ShaderUnit vertShader(ShaderStage::Vertex, vertexSource);
            ShaderUnit fragShader(ShaderStage::Fragment, fragmentSource);

            program.AttachShader(std::move(vertShader));
            program.AttachShader(std::move(fragShader));
            program.Link();

            return program;
        }

        static ShaderProgram FromFilesWithGeometry(const String &vertexPath,
                                                   const String &fragmentPath,
                                                   const String &geometryPath,
                                                   const ShaderConfig &config = ShaderConfig::Default()) {
            ShaderProgram program(config);

            ShaderUnit vertShader = ShaderUnit::FromFile(ShaderStage::Vertex, vertexPath);
            ShaderUnit fragShader = ShaderUnit::FromFile(ShaderStage::Fragment, fragmentPath);
            ShaderUnit geomShader = ShaderUnit::FromFile(ShaderStage::Geometry, geometryPath);

            program.AttachShader(std::move(vertShader));
            program.AttachShader(std::move(fragShader));
            program.AttachShader(std::move(geomShader));
            program.Link();

            return program;
        }

    private:
        mutable std::unordered_map<String, GLint> m_UniformCache;
        std::unordered_set<ShaderStage> m_AttachedStages;
        Vector<GLuint> m_AttachedShaderIDs;
        mutable std::unordered_set<String> m_WarnedUniforms;
        ShaderConfig m_Config;

        GLint GetUniformLocation(const String &name) const {
            if (m_Config.cacheUniforms) {
                auto it = m_UniformCache.find(name);
                if (it != m_UniformCache.end())
                    return it->second;
            }

            if (!m_ID)
                return -1;

            const GLint loc = glGetUniformLocation(m_ID, name.c_str());

            if (loc == -1 && m_Config.warnOnMissingUniform && !m_WarnedUniforms.contains(name)) {
                Logger::Error() << "Warning: uniform '" << name << "' doesn't exist!";
                m_WarnedUniforms.insert(name);
            }

            if (m_Config.cacheUniforms)
                m_UniformCache.emplace(name, loc);

            return loc;
        }

        void CacheAllUniforms() const {
            if (!m_ID) {
                Logger::Error() << "Attempt to cache uniforms on invalid program!";
                return;
            }

            GLint uniformCount = 0;
            glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &uniformCount);

            constexpr size_t MaxUniformNameLength = 256;

            for (GLint i = 0; i < uniformCount; ++i) {
                Vector<char> nameBuf(MaxUniformNameLength);
                GLsizei length = 0;
                GLint size = 0;
                GLenum type = 0;

                glGetActiveUniform(m_ID, i, static_cast<GLsizei>(nameBuf.size()), &length, &size, &type,
                                   nameBuf.data());

                if (length <= 0)
                    continue;

                String name(nameBuf.data(), static_cast<size_t>(length));
                const GLint location = glGetUniformLocation(m_ID, name.c_str());
                if (location != -1)
                    m_UniformCache[name] = location;
            }
        }
    };
}

#endif // ASHEN_SHADER_H