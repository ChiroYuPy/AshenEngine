#ifndef ASHEN_SHADER_H
#define ASHEN_SHADER_H

#include <iostream>
#include <stdexcept>
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Ashen/math/Math.h"
#include "Ashen/renderer/Bindable.h"

namespace ash {
    enum class ShaderType : GLenum {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
    };

    class ShaderUnit {
    public:
        ShaderUnit(const ShaderType type, const std::string &source)
            : m_Type(type) { Compile(source); }

        ~ShaderUnit() { if (m_ID) glDeleteShader(m_ID); }

        ShaderUnit(const ShaderUnit &) = delete;

        ShaderUnit &operator=(const ShaderUnit &) = delete;

        [[nodiscard]] GLuint ID() const { return m_ID; }
        [[nodiscard]] ShaderType Type() const { return m_Type; }

    private:
        GLuint m_ID = 0;
        ShaderType m_Type;

        void Compile(const std::string &source) {
            m_ID = glCreateShader(static_cast<GLenum>(m_Type));
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
                throw std::runtime_error("Shader compilation failed: " + infoLog);
            }
        }
    };

    class ShaderProgram final : public Bindable {
    public:
        ShaderProgram() = default;

        ~ShaderProgram() override { if (m_ID) glDeleteProgram(m_ID); }

        ShaderProgram(const ShaderProgram &) = delete;

        ShaderProgram &operator=(const ShaderProgram &) = delete;

        ShaderProgram(ShaderProgram &&other) noexcept
            : m_ID(other.m_ID), m_UniformCache(std::move(other.m_UniformCache)) {
            other.m_ID = 0;
        }

        ShaderProgram &operator=(ShaderProgram &&other) noexcept {
            if (this != &other) {
                if (m_ID) glDeleteProgram(m_ID);
                m_ID = other.m_ID;
                m_UniformCache = std::move(other.m_UniformCache);
                other.m_ID = 0;
            }
            return *this;
        }

        void Bind() const override { glUseProgram(m_ID); }
        void Unbind() const override { glUseProgram(0); }

        void AttachShader(const ShaderUnit &shader) {
            const ShaderType type = shader.Type();
            if (m_AttachedTypes.contains(type))
                throw std::runtime_error("ShaderProgram already has a shader of this type attached!");

            if (!m_ID) m_ID = glCreateProgram();
            glAttachShader(m_ID, shader.ID());
            m_AttachedTypes.insert(type);
            m_AttachedShaderIDs.push_back(shader.ID());
        }

        void Link() {
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
                throw std::runtime_error("ShaderProgram linking failed: " + infoLog);
            }

            // Détacher les shaders après le link
            for (GLuint id: m_AttachedShaderIDs)
                glDetachShader(m_ID, id);
            m_AttachedShaderIDs.clear();
        }

        [[nodiscard]] GLuint ID() const { return m_ID; }

        void SetInt(const std::string &name, const int value) const { glUniform1i(GetUniformLocation(name), value); }

        void SetFloat(const std::string &name, const float value) const {
            glUniform1f(GetUniformLocation(name), value);
        }

        void SetVec2(const std::string &name, const Vec2 &v) const {
            glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(v));
        }

        void SetVec3(const std::string &name, const Vec3 &v) const {
            glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(v));
        }

        void SetVec4(const std::string &name, const Vec4 &v) const {
            glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(v));
        }

        void SetMat3(const std::string &name, const Mat3 &m) const {
            glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
        }

        void SetMat4(const std::string &name, const Mat4 &m) const {
            glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
        }

    private:
        GLuint m_ID = 0;
        mutable std::unordered_map<std::string, GLint> m_UniformCache;
        std::unordered_set<ShaderType> m_AttachedTypes;
        std::vector<GLuint> m_AttachedShaderIDs;
        mutable std::unordered_set<std::string> m_WarnedUniforms;

        GLint GetUniformLocation(const std::string &name) const {
            if (m_UniformCache.contains(name)) return m_UniformCache[name];
            const GLint loc = glGetUniformLocation(m_ID, name.c_str());
            if (loc == -1 && !m_WarnedUniforms.contains(name)) {
                std::cerr << "Warning: uniform '" << name << "' doesn't exist!\n";
                m_WarnedUniforms.insert(name);
            }
            m_UniformCache[name] = loc;
            return loc;
        }
    };
}

#endif // ASHEN_SHADER_H