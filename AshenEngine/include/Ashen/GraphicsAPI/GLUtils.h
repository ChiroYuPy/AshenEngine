#ifndef ASHEN_GLUTILS_H
#define ASHEN_GLUTILS_H

#include <glad/glad.h>

namespace ash {
    enum class DebugSeverity : GLenum {
        High = GL_DEBUG_SEVERITY_HIGH,
        Medium = GL_DEBUG_SEVERITY_MEDIUM,
        Low = GL_DEBUG_SEVERITY_LOW,
        Notification = GL_DEBUG_SEVERITY_NOTIFICATION
    };

    enum class DebugSource : GLenum {
        API = GL_DEBUG_SOURCE_API,
        WindowSystem = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
        ShaderCompiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
        ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
        Application = GL_DEBUG_SOURCE_APPLICATION,
        Other = GL_DEBUG_SOURCE_OTHER
    };

    enum class DebugType : GLenum {
        Error = GL_DEBUG_TYPE_ERROR,
        DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
        UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
        Portability = GL_DEBUG_TYPE_PORTABILITY,
        Performance = GL_DEBUG_TYPE_PERFORMANCE,
        Marker = GL_DEBUG_TYPE_MARKER,
        Other = GL_DEBUG_TYPE_OTHER
    };

    const char *GLDebugSourceToString(GLenum source);

    const char *GLDebugTypeToString(GLenum type);

    const char *GLDebugSeverityToString(GLenum severity);

    void InitOpenGLDebugMessageCallback();

    void SetDebugMessageFilter(DebugSeverity minSeverity = DebugSeverity::Medium);
}

#endif //ASHEN_GLUTILS_H