#ifndef ASHEN_EVENTDISPATCHER_H
#define ASHEN_EVENTDISPATCHER_H

#include "Event.h"

namespace ash {
    class EventDispatcher {
    public:
        explicit EventDispatcher(Event &event) : mEvent(event) {
        }

        template<typename T, typename Func>
        bool Dispatch(const Func &func) {
            if (mEvent.GetType() == T::GetStaticType()) {
                if (func(static_cast<T &>(mEvent)))
                    mEvent.SetHandled();
                return true;
            }
            return false;
        }

    private:
        Event &mEvent;
    };
}

#endif //ASHEN_EVENTDISPATCHER_H