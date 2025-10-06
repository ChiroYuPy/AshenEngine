#include "Ashen/GraphicsAPI/GLUtils.h"

#include <format>

#include "Ashen/Core/Logger.h"

namespace ash {
    static auto s_MinSeverity = DebugSeverity::Medium;

    const char *GLDebugSourceToString(const GLenum source) {
        switch (source) {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION: return "Application";
            case GL_DEBUG_SOURCE_OTHER:
            default: return "Other";
        }
    }

    const char *GLDebugTypeToString(const GLenum type) {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
            case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
            case GL_DEBUG_TYPE_MARKER: return "Marker";
            case GL_DEBUG_TYPE_OTHER: return "Other";
            default: return "Unknown";
        }
    }

    const char *GLDebugSeverityToString(const GLenum severity) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            default: return "UNKNOWN";
        }
    }

    static void GLDebugCallback(const GLenum source,
                                const GLenum type,
                                const GLuint id,
                                const GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam) {
        // Filter based on minimum severity
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION &&
            s_MinSeverity != DebugSeverity::Notification) {
            return;
        }
        if (severity == GL_DEBUG_SEVERITY_LOW &&
            (s_MinSeverity == DebugSeverity::Medium || s_MinSeverity == DebugSeverity::High)) {
            return;
        }
        if (severity == GL_DEBUG_SEVERITY_MEDIUM && s_MinSeverity == DebugSeverity::High) {
            return;
        }

        const char *sourceStr = GLDebugSourceToString(source);
        const char *typeStr = GLDebugTypeToString(type);
        const char *severityStr = GLDebugSeverityToString(severity);

        Logger::error() << std::format("[OpenGL] [{}] {} (ID: {}) [{}]: {}",
                                       severityStr, typeStr, id, sourceStr, message);
    }

    void InitOpenGLDebugMessageCallback() {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugCallback, nullptr);
    }

    void SetDebugMessageFilter(const DebugSeverity minSeverity) {
        s_MinSeverity = minSeverity;
    }
}