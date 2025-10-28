#ifndef ASHEN_EVENTDISPATCHER_H
#define ASHEN_EVENTDISPATCHER_H

#include "Event.h"

namespace ash {

    class EventDispatcher {
    public:
        explicit EventDispatcher(Event& event) : mEvent(event) {}

        template<typename T, typename Func>
        bool Dispatch(const Func& func) {
            if (mEvent.GetType() == T::GetStaticType()) {
                mEvent.Handled = func(static_cast<T&>(mEvent));
                return true;
            }
            return false;
        }
    private:
        Event& mEvent;
    };
}

#endif //ASHEN_EVENTDISPATCHER_H