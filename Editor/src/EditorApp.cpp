#include "EditorLayer.h"
#include "Ashen/Core/Application.h"

namespace ash {
    class EditorApplication : public Application {
    public:
        explicit EditorApplication(const ApplicationCommandLineArgs args)
            : Application({
                .Name = "AshenEngine Editor",
                .Version = "0.1.0",
                .ResourceDirectory = "resources",
                .CommandLineArgs = args
            }) {
            // Push the editor layer
            PushLayer<EditorLayer>();
        }

        ~EditorApplication() = default;
    };
}

// Entry point
ash::Application* ash::CreateApplication(const ApplicationCommandLineArgs args) {
    return new EditorApplication(args);
}

#include "Ashen/Core/EntryPoint.h"