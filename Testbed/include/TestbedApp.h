#ifndef TESTBED_TESTBEDAPP_H
#define TESTBED_TESTBEDAPP_H

#include "Ashen/Core/Application.h"

namespace ash {
    class TestbedApp : public Application {
    public:
        explicit TestbedApp(const ApplicationSettings &settings);

        static TestbedApp &Get() {
            return static_cast<TestbedApp &>(Application::Get());
        }
    };
}

#endif //TESTBED_TESTBEDAPP_H