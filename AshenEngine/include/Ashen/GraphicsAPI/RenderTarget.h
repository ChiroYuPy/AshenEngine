#ifndef ASHEN_RENDERTARGET_H
#define ASHEN_RENDERTARGET_H

#include "Ashen/Core/Types.h"

namespace ash {
    class RenderTarget {
    public:
        virtual void Bind() const = 0;

        virtual void Unbind() const = 0;

        virtual u32 GetWidth() const = 0;

        virtual u32 GetHeight() const = 0;

        virtual ~RenderTarget() = default;
    };
}

#endif //ASHEN_RENDERTARGET_H