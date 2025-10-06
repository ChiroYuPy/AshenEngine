#include "Ashen/renderer/Renderer3D.h"
#include "Ashen/renderer/Camera.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace ash {
    std::vector<Line3D> Renderer3D::s_Lines;
    glm::mat4 Renderer3D::s_ViewProjection = glm::mat4(1.0f);
    uint32_t Renderer3D::s_VAO = 0;
    uint32_t Renderer3D::s_VBO = 0;
    uint32_t Renderer3D::s_ShaderProgram = 0;

    static auto vertexShaderSrc = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec4 aColor;

    uniform mat4 u_ViewProjection;

    out vec4 vColor;

    void main() {
        vColor = aColor;
        gl_Position = u_ViewProjection * vec4(aPos, 1.0);
    }
    )";

    static auto fragmentShaderSrc = R"(
    #version 330 core
    in vec4 vColor;
    out vec4 FragColor;

    void main() {
        FragColor = vColor;
    }
    )";

    void Renderer3D::Init() {
        // Create VAO/VBO
        glGenVertexArrays(1, &s_VAO);
        glGenBuffers(1, &s_VBO);

        glBindVertexArray(s_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 7 * 2 * 10000, nullptr, GL_DYNAMIC_DRAW); // max 10000 lines

        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void *) 0);
        // color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void *) (sizeof(float) * 3));

        glBindVertexArray(0);

        // Compile simple shader
        auto compile = [](uint32_t type, const char *src) -> uint32_t {
            uint32_t shader = glCreateShader(type);
            glShaderSource(shader, 1, &src, nullptr);
            glCompileShader(shader);
            int success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                std::cerr << "Shader compile error: " << infoLog << std::endl;
            }
            return shader;
        };

        uint32_t vs = compile(GL_VERTEX_SHADER, vertexShaderSrc);
        uint32_t fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSrc);

        s_ShaderProgram = glCreateProgram();
        glAttachShader(s_ShaderProgram, vs);
        glAttachShader(s_ShaderProgram, fs);
        glLinkProgram(s_ShaderProgram);

        int success;
        glGetProgramiv(s_ShaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(s_ShaderProgram, 512, nullptr, infoLog);
            std::cerr << "Shader link error: " << infoLog << std::endl;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void Renderer3D::Shutdown() {
        glDeleteVertexArrays(1, &s_VAO);
        glDeleteBuffers(1, &s_VBO);
        glDeleteProgram(s_ShaderProgram);
    }

    void Renderer3D::BeginScene(const Camera &camera) {
        s_ViewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();
    }

    void Renderer3D::EndScene() {
        FlushLines();
        s_Lines.clear();
    }

    void Renderer3D::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color) {
        s_Lines.push_back({start, end, color});
    }

    void Renderer3D::FlushLines() {
        if (s_Lines.empty()) return;

        glBindVertexArray(s_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, s_VBO);

        std::vector<float> bufferData;
        bufferData.reserve(s_Lines.size() * 7 * 2);
        for (const auto &[start, end, color]: s_Lines) {
            // start vertex
            bufferData.push_back(start.x);
            bufferData.push_back(start.y);
            bufferData.push_back(start.z);
            bufferData.push_back(color.r);
            bufferData.push_back(color.g);
            bufferData.push_back(color.b);
            bufferData.push_back(color.a);
            // end vertex
            bufferData.push_back(end.x);
            bufferData.push_back(end.y);
            bufferData.push_back(end.z);
            bufferData.push_back(color.r);
            bufferData.push_back(color.g);
            bufferData.push_back(color.b);
            bufferData.push_back(color.a);
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, bufferData.size() * sizeof(float), bufferData.data());

        glUseProgram(s_ShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(s_ShaderProgram, "u_ViewProjection"), 1, GL_FALSE,
                           glm::value_ptr(s_ViewProjection));

        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(s_Lines.size() * 2));

        glBindVertexArray(0);
    }
}