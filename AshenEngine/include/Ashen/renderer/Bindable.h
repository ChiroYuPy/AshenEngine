#ifndef ASHEN_BINDABLE_H
#define ASHEN_BINDABLE_H

namespace pixl {
    class Bindable {
    public:
        virtual ~Bindable() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        Bindable(const Bindable&) = delete;
        Bindable& operator=(const Bindable&) = delete;

    protected:
        Bindable() = default;

        Bindable(Bindable&&) = default;
        Bindable& operator=(Bindable&&) = default;
    };

    class BindGuard {
    public:
        explicit BindGuard(const Bindable& bindable) : m_Bindable(bindable) {
            m_Bindable.Bind();
        }

        ~BindGuard() {
            m_Bindable.Unbind();
        }

        BindGuard(const BindGuard&) = delete;
        BindGuard& operator=(const BindGuard&) = delete;
        BindGuard(BindGuard&&) = delete;
        BindGuard& operator=(BindGuard&&) = delete;

    private:
        const Bindable& m_Bindable;
    };
}

#endif //ASHEN_BINDABLE_H