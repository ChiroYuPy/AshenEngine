#ifndef ASHEN_GLENUMS_H
#define ASHEN_GLENUMS_H

#include <glad/glad.h>

namespace ash {
    enum class TextureTarget : GLenum {
        Texture1D = GL_TEXTURE_1D,
        Texture2D = GL_TEXTURE_2D,
        Texture3D = GL_TEXTURE_3D,
        CubeMap = GL_TEXTURE_CUBE_MAP,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Texture2DArray = GL_TEXTURE_2D_ARRAY,
        TextureRectangle = GL_TEXTURE_RECTANGLE,
        CubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
        Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
        Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    };

    enum class TextureFormat : GLenum {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        Depth = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL,

        R8 = GL_R8,
        RG8 = GL_RG8,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,

        R16 = GL_R16,
        RG16 = GL_RG16,
        RGB16 = GL_RGB16,
        RGBA16 = GL_RGBA16,

        R16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        R32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,

        R8I = GL_R8I,
        RG8I = GL_RG8I,
        RGB8I = GL_RGB8I,
        RGBA8I = GL_RGBA8I,
        R16I = GL_R16I,
        RG16I = GL_RG16I,
        RGB16I = GL_RGB16I,
        RGBA16I = GL_RGBA16I,
        R32I = GL_R32I,
        RG32I = GL_RG32I,
        RGB32I = GL_RGB32I,
        RGBA32I = GL_RGBA32I,

        R8UI = GL_R8UI,
        RG8UI = GL_RG8UI,
        RGB8UI = GL_RGB8UI,
        RGBA8UI = GL_RGBA8UI,

        Depth16 = GL_DEPTH_COMPONENT16,
        Depth24 = GL_DEPTH_COMPONENT24,
        Depth32 = GL_DEPTH_COMPONENT32,
        Depth32F = GL_DEPTH_COMPONENT32F,
        Depth24Stencil8 = GL_DEPTH24_STENCIL8,
        Depth32FStencil8 = GL_DEPTH32F_STENCIL8,

        SRGB8 = GL_SRGB8,
        SRGB8Alpha8 = GL_SRGB8_ALPHA8
    };

    enum class TextureWrap : GLenum {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
    };

    enum class TextureFilter : GLenum {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    enum class PixelDataType : GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        Float = GL_FLOAT,
        HalfFloat = GL_HALF_FLOAT,
        UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,
        UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
        UnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,
        UnsignedInt1010102 = GL_UNSIGNED_INT_10_10_10_2
    };

    enum class CubeMapFace : GLenum {
        PositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        NegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        PositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        NegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        PositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        NegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    enum class BufferTarget : GLenum {
        Array = GL_ARRAY_BUFFER,
        ElementArray = GL_ELEMENT_ARRAY_BUFFER,
        Uniform = GL_UNIFORM_BUFFER,
        ShaderStorage = GL_SHADER_STORAGE_BUFFER,
        CopyRead = GL_COPY_READ_BUFFER,
        CopyWrite = GL_COPY_WRITE_BUFFER,
        PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
        PixelPack = GL_PIXEL_PACK_BUFFER,
        TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
        Texture = GL_TEXTURE_BUFFER,
        DrawIndirect = GL_DRAW_INDIRECT_BUFFER,
        AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
        DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER
    };

    enum class BufferUsage : GLenum {
        StreamDraw = GL_STREAM_DRAW,
        StreamRead = GL_STREAM_READ,
        StreamCopy = GL_STREAM_COPY,
        StaticDraw = GL_STATIC_DRAW,
        StaticRead = GL_STATIC_READ,
        StaticCopy = GL_STATIC_COPY,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY
    };

    enum class BufferAccess : GLenum {
        ReadOnly = GL_READ_ONLY,
        WriteOnly = GL_WRITE_ONLY,
        ReadWrite = GL_READ_WRITE
    };

    enum class ShaderStage : GLenum {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        TessControl = GL_TESS_CONTROL_SHADER,
        TessEvaluation = GL_TESS_EVALUATION_SHADER,
        Compute = GL_COMPUTE_SHADER
    };

    enum class FramebufferTarget : GLenum {
        Read = GL_READ_FRAMEBUFFER,
        Draw = GL_DRAW_FRAMEBUFFER,
        ReadDraw = GL_FRAMEBUFFER
    };

    enum class FramebufferAttachment : GLenum {
        Color0 = GL_COLOR_ATTACHMENT0,
        Color1 = GL_COLOR_ATTACHMENT1,
        Color2 = GL_COLOR_ATTACHMENT2,
        Color3 = GL_COLOR_ATTACHMENT3,
        Color4 = GL_COLOR_ATTACHMENT4,
        Color5 = GL_COLOR_ATTACHMENT5,
        Color6 = GL_COLOR_ATTACHMENT6,
        Color7 = GL_COLOR_ATTACHMENT7,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT,
        DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
    };

    enum class RenderbufferFormat : GLenum {
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,
        Depth16 = GL_DEPTH_COMPONENT16,
        Depth24 = GL_DEPTH_COMPONENT24,
        Depth32F = GL_DEPTH_COMPONENT32F,
        Depth24Stencil8 = GL_DEPTH24_STENCIL8,
        Depth32FStencil8 = GL_DEPTH32F_STENCIL8,
        Stencil8 = GL_STENCIL_INDEX8
    };

    enum class PrimitiveType : GLenum {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        LineLoop = GL_LINE_LOOP,
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN,
        LinesAdjacency = GL_LINES_ADJACENCY,
        LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
        TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
        TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
        Patches = GL_PATCHES
    };

    enum class IndexType : GLenum {
        UnsignedByte = GL_UNSIGNED_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        UnsignedInt = GL_UNSIGNED_INT
    };

    enum class Capability : GLenum {
        Blend = GL_BLEND,
        CullFace = GL_CULL_FACE,
        DepthTest = GL_DEPTH_TEST,
        Dither = GL_DITHER,
        PolygonOffsetFill = GL_POLYGON_OFFSET_FILL,
        SampleAlphaToCoverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
        SampleCoverage = GL_SAMPLE_COVERAGE,
        ScissorTest = GL_SCISSOR_TEST,
        StencilTest = GL_STENCIL_TEST,
        Multisample = GL_MULTISAMPLE,
        ProgramPointSize = GL_PROGRAM_POINT_SIZE,
        LineSmooth = GL_LINE_SMOOTH,
        PolygonSmooth = GL_POLYGON_SMOOTH,
        DepthClamp = GL_DEPTH_CLAMP,
        PrimitiveRestart = GL_PRIMITIVE_RESTART,
        FramebufferSRGB = GL_FRAMEBUFFER_SRGB
    };

    enum class BlendFactor : GLenum {
        Zero = GL_ZERO,
        One = GL_ONE,
        SrcColor = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        DstColor = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
        SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE
    };

    enum class BlendEquation : GLenum {
        Add = GL_FUNC_ADD,
        Subtract = GL_FUNC_SUBTRACT,
        ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
        Min = GL_MIN,
        Max = GL_MAX
    };

    enum class DepthFunc : GLenum {
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        LessOrEqual = GL_LEQUAL,
        Greater = GL_GREATER,
        NotEqual = GL_NOTEQUAL,
        GreaterOrEqual = GL_GEQUAL,
        Always = GL_ALWAYS
    };

    enum class StencilOp : GLenum {
        Keep = GL_KEEP,
        Zero = GL_ZERO,
        Replace = GL_REPLACE,
        Increment = GL_INCR,
        IncrementWrap = GL_INCR_WRAP,
        Decrement = GL_DECR,
        DecrementWrap = GL_DECR_WRAP,
        Invert = GL_INVERT
    };

    enum class CullFaceMode : GLenum {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK
    };

    enum class FrontFace : GLenum {
        Clockwise = GL_CW,
        CounterClockwise = GL_CCW
    };

    enum class PolygonMode : GLenum {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL
    };

    enum class VertexAttribType : GLenum {
        Byte = GL_BYTE,
        UnsignedByte = GL_UNSIGNED_BYTE,
        Short = GL_SHORT,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Int = GL_INT,
        UnsignedInt = GL_UNSIGNED_INT,
        HalfFloat = GL_HALF_FLOAT,
        Float = GL_FLOAT,
        Double = GL_DOUBLE,
        Fixed = GL_FIXED,
        Int2101010Rev = GL_INT_2_10_10_10_REV,
        UnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,
        UnsignedInt10F11F11FRev = GL_UNSIGNED_INT_10F_11F_11F_REV
    };

    enum class ClearBuffer : GLbitfield {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT,
        ColorDepth = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        ColorStencil = GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        DepthStencil = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        All = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
    };

    constexpr ClearBuffer operator|(ClearBuffer a, ClearBuffer b) {
        return static_cast<ClearBuffer>(
            static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b)
        );
    }
}

#endif //ASHEN_GLENUMS_H
