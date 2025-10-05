#ifndef ASHEN_ENTRYPOINT_H
#define ASHEN_ENTRYPOINT_H

#include "Ashen/core/Application.h"

#ifndef ASHEN_NO_ENTRY_POINT

int main(const int argc, char **argv) {
    const auto app = ash::CreateApplication({argc, argv});
    app->Run();
    delete app;
}

#endif //ASHEN_NO_ENTRY_POINT

#endif //ASHEN_ENTRYPOINT_H