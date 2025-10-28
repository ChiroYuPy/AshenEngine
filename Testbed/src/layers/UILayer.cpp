#include "layers/UILayer.h"

#include "Ashen/Graphics/UI/UISystem.h"
#include "Ashen/Graphics/UI/UIWidgets.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Events/EventDispatcher.h"

namespace ash {
    void UILayer::OnAttach() {
        // Créer la caméra orthographique pour UI avec origin en bas à gauche
        mCamera = MakeRef<OrthographicCamera>(1280.0f, 720.0f, OrthographicCamera::OriginMode::BottomLeft);

        // Initialiser UISystem
        UISystem::Init();
        UISystem::SetCamera(mCamera);
        UISystem::SetScreenSize(Vec2(1280.0f, 720.0f));

        // Créer le menu principal par défaut
        mCurrentState = UIState::MainMenu;
        CreateMainMenu();

        Logger::Info("UILayer attached");
    }

    void UILayer::OnUpdate(const float deltaTime) {
        UISystem::BeginFrame();
        UISystem::Update(deltaTime);
        UISystem::EndFrame();

        // Tests de changement d'état avec les touches
        if (Input::IsKeyPressed(Key::F1)) {
            SetUIState(UIState::MainMenu);
        }
        if (Input::IsKeyPressed(Key::F2)) {
            SetUIState(UIState::GameHUD);
        }
        if (Input::IsKeyPressed(Key::F3)) {
            SetUIState(UIState::PauseMenu);
        }
    }

    void UILayer::OnRender() {
        // BeginScene avec la caméra UI (origin en bas à gauche)
        Renderer2D::BeginScene(*mCamera);

        // Rendre l'UI
        UISystem::Render();

        // Test de rendu direct - (0,0) est maintenant en bas à gauche
        // Renderer2D::DrawRect(Vec3(10, 10, 0), Vec2(100, 100), Vec4(1.0f, 0.0f, 0.0f, 1.0f));

        Renderer2D::EndScene();
    }

    void UILayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        // Gérer le redimensionnement
        dispatcher.Dispatch<WindowResizeEvent>([&](const WindowResizeEvent &e) {
            float width = static_cast<float>(e.GetWidth());
            float height = static_cast<float>(e.GetHeight());

            mCamera->OnResize(e.GetWidth(), e.GetHeight());
            UISystem::SetScreenSize(Vec2(width, height));

            // Recréer l'UI avec les nouvelles dimensions
            SetUIState(mCurrentState);

            return false;
        });

        // Passer l'événement au système UI
        UISystem::HandleEvent(event);
    }

    void UILayer::OnDetach() {
        UISystem::Shutdown();
    }

    void UILayer::SetUIState(UIState state) {
        mCurrentState = state;

        switch (state) {
            case UIState::MainMenu:
                CreateMainMenu();
                Logger::Info("Switched to Main Menu");
                break;
            case UIState::GameHUD:
                CreateGameHUD();
                Logger::Info("Switched to Game HUD");
                break;
            case UIState::PauseMenu:
                CreatePauseMenu();
                Logger::Info("Switched to Pause Menu");
                break;
        }
    }

    void UILayer::CreateMainMenu() {
        auto root = std::make_shared<Panel>("MainMenu");
        Vec2 screenSize = UISystem::GetScreenSize();
        root->SetSize(screenSize);
        root->SetBgColor(Vec4(0.1f, 0.1f, 0.15f, 1.0f));

        // Container principal centré
        auto centerContainer = std::make_shared<MarginContainer>("CenterContainer");
        centerContainer->SetSize(Vec2(400.0f, 500.0f));
        centerContainer->SetPosition((screenSize - Vec2(400.0f, 500.0f)) * 0.5f);
        centerContainer->SetMargin(20.0f);
        root->AddChild(centerContainer);

        // VBox pour empiler les éléments
        auto vbox = std::make_shared<VBoxContainer>("VBox");
        vbox->SetSize(Vec2(360.0f, 460.0f));
        vbox->SetSpacing(15.0f);
        centerContainer->AddChild(vbox);

        // Logo/Titre
        auto titlePanel = std::make_shared<Panel>("TitlePanel");
        titlePanel->SetSize(Vec2(360.0f, 100.0f));
        titlePanel->SetBgColor(Vec4(0.2f, 0.25f, 0.35f, 1.0f));
        titlePanel->SetCornerRadius(8.0f);
        vbox->AddChild(titlePanel);

        auto title = std::make_shared<Label>("Title");
        title->SetText("AWESOME GAME");
        title->SetFontSize(32.0f);
        title->SetTextColor(Vec4(1.0f, 0.9f, 0.3f, 1.0f));
        title->SetAlignment(1); // Center
        title->SetSize(Vec2(340.0f, 40.0f));
        title->SetPosition(Vec2(10.0f, 30.0f));
        titlePanel->AddChild(title);

        // Bouton Play
        auto playButton = std::make_shared<Button>("PlayButton");
        playButton->SetText("PLAY");
        playButton->SetSize(Vec2(360.0f, 50.0f));
        playButton->OnPressed = [this]() {
            Logger::Info("Play button pressed!");
            SetUIState(UIState::GameHUD);
        };
        vbox->AddChild(playButton);

        // Bouton Options
        auto optionsButton = std::make_shared<Button>("OptionsButton");
        optionsButton->SetText("OPTIONS");
        optionsButton->SetSize(Vec2(360.0f, 50.0f));
        optionsButton->OnPressed = []() {
            Logger::Info("Options button pressed!");
        };
        vbox->AddChild(optionsButton);

        // Bouton Quit
        auto quitButton = std::make_shared<Button>("QuitButton");
        quitButton->SetText("QUIT");
        quitButton->SetSize(Vec2(360.0f, 50.0f));
        quitButton->OnPressed = []() {
            Logger::Info("Quit button pressed!");
        };
        vbox->AddChild(quitButton);

        // Panel d'informations en bas
        auto infoPanel = std::make_shared<Panel>("InfoPanel");
        infoPanel->SetSize(Vec2(360.0f, 80.0f));
        infoPanel->SetBgColor(Vec4(0.15f, 0.15f, 0.2f, 0.8f));
        vbox->AddChild(infoPanel);

        auto infoLabel = std::make_shared<Label>("InfoLabel");
        infoLabel->SetText("Version 1.0.0 - F1/F2/F3 to switch");
        infoLabel->SetFontSize(12.0f);
        infoLabel->SetTextColor(Vec4(0.6f, 0.6f, 0.6f, 1.0f));
        infoLabel->SetAlignment(1); // Center
        infoLabel->SetSize(Vec2(340.0f, 20.0f));
        infoLabel->SetPosition(Vec2(10.0f, 30.0f));
        infoPanel->AddChild(infoLabel);

        UISystem::SetRoot(root);
    }

    void UILayer::CreateGameHUD() {
        auto root = std::make_shared<Node>("GameHUD");

        // Panel supérieur pour le score et la vie
        auto topPanel = std::make_shared<Panel>("TopPanel");
        topPanel->SetPosition(Vec2(10.0f, 10.0f));
        topPanel->SetSize(Vec2(300.0f, 60.0f));
        topPanel->SetBgColor(Vec4(0.1f, 0.1f, 0.15f, 0.9f));
        topPanel->SetCornerRadius(6.0f);
        root->AddChild(topPanel);

        auto topHBox = std::make_shared<HBoxContainer>("TopHBox");
        topHBox->SetSize(Vec2(280.0f, 40.0f));
        topHBox->SetPosition(Vec2(10.0f, 10.0f));
        topHBox->SetSpacing(20.0f);
        topPanel->AddChild(topHBox);

        // Score
        auto scoreLabel = std::make_shared<Label>("ScoreLabel");
        scoreLabel->SetText("Score: 0");
        scoreLabel->SetFontSize(18.0f);
        scoreLabel->SetTextColor(Vec4(1.0f, 1.0f, 0.3f, 1.0f));
        scoreLabel->SetSize(Vec2(120.0f, 30.0f));
        topHBox->AddChild(scoreLabel);

        // Barre de vie
        auto healthBar = std::make_shared<ProgressBar>("HealthBar");
        healthBar->SetValue(100.0f);
        healthBar->SetSize(Vec2(120.0f, 30.0f));
        healthBar->SetShowPercentage(false);
        topHBox->AddChild(healthBar);

        // Panel de boutons en bas à droite
        Vec2 screenSize = UISystem::GetScreenSize();
        auto bottomRightPanel = std::make_shared<Panel>("BottomRightPanel");
        bottomRightPanel->SetPosition(screenSize - Vec2(210.0f, 160.0f));
        bottomRightPanel->SetSize(Vec2(200.0f, 150.0f));
        bottomRightPanel->SetBgColor(Vec4(0.1f, 0.1f, 0.15f, 0.8f));
        bottomRightPanel->SetCornerRadius(6.0f);
        root->AddChild(bottomRightPanel);

        auto buttonVBox = std::make_shared<VBoxContainer>("ButtonVBox");
        buttonVBox->SetSize(Vec2(180.0f, 130.0f));
        buttonVBox->SetPosition(Vec2(10.0f, 10.0f));
        buttonVBox->SetSpacing(10.0f);
        bottomRightPanel->AddChild(buttonVBox);

        // Boutons d'action
        auto attackButton = std::make_shared<Button>("AttackButton");
        attackButton->SetText("ATTACK");
        attackButton->SetSize(Vec2(180.0f, 35.0f));
        attackButton->OnPressed = []() {
            Logger::Info("Attack!");
        };
        buttonVBox->AddChild(attackButton);

        auto defendButton = std::make_shared<Button>("DefendButton");
        defendButton->SetText("DEFEND");
        defendButton->SetSize(Vec2(180.0f, 35.0f));
        defendButton->OnPressed = []() {
            Logger::Info("Defend!");
        };
        buttonVBox->AddChild(defendButton);

        auto skillButton = std::make_shared<Button>("SkillButton");
        skillButton->SetText("SPECIAL");
        skillButton->SetSize(Vec2(180.0f, 35.0f));
        skillButton->OnPressed = []() {
            Logger::Info("Special skill!");
        };
        buttonVBox->AddChild(skillButton);

        // Pause button en haut à droite
        auto pauseButton = std::make_shared<Button>("PauseButton");
        pauseButton->SetText("||");
        pauseButton->SetPosition(screenSize - Vec2(60.0f, -10.0f));
        pauseButton->SetSize(Vec2(50.0f, 50.0f));
        pauseButton->OnPressed = [this]() {
            Logger::Info("Pause game!");
            SetUIState(UIState::PauseMenu);
        };
        root->AddChild(pauseButton);

        UISystem::SetRoot(root);
    }

    void UILayer::CreatePauseMenu() {
        Vec2 screenSize = UISystem::GetScreenSize();

        auto root = std::make_shared<Panel>("PauseMenu");
        root->SetSize(screenSize);
        root->SetBgColor(Vec4(0.0f, 0.0f, 0.0f, 0.7f)); // Semi-transparent

        auto centerPanel = std::make_shared<Panel>("CenterPanel");
        centerPanel->SetPosition((screenSize - Vec2(300.0f, 350.0f)) * 0.5f);
        centerPanel->SetSize(Vec2(300.0f, 350.0f));
        centerPanel->SetBgColor(Vec4(0.15f, 0.15f, 0.2f, 1.0f));
        centerPanel->SetCornerRadius(8.0f);
        root->AddChild(centerPanel);

        auto vbox = std::make_shared<VBoxContainer>("VBox");
        vbox->SetSize(Vec2(280.0f, 330.0f));
        vbox->SetPosition(Vec2(10.0f, 10.0f));
        vbox->SetSpacing(10.0f);
        centerPanel->AddChild(vbox);

        // Titre
        auto title = std::make_shared<Label>("Title");
        title->SetText("PAUSED");
        title->SetFontSize(28.0f);
        title->SetAlignment(1); // Center
        title->SetSize(Vec2(280.0f, 40.0f));
        vbox->AddChild(title);

        // Boutons
        auto resumeButton = std::make_shared<Button>("ResumeButton");
        resumeButton->SetText("RESUME");
        resumeButton->SetSize(Vec2(280.0f, 50.0f));
        resumeButton->OnPressed = [this]() {
            Logger::Info("Resume game!");
            SetUIState(UIState::GameHUD);
        };
        vbox->AddChild(resumeButton);

        auto restartButton = std::make_shared<Button>("RestartButton");
        restartButton->SetText("RESTART");
        restartButton->SetSize(Vec2(280.0f, 50.0f));
        restartButton->OnPressed = []() {
            Logger::Info("Restart game!");
        };
        vbox->AddChild(restartButton);

        auto optionsButton = std::make_shared<Button>("OptionsButton");
        optionsButton->SetText("OPTIONS");
        optionsButton->SetSize(Vec2(280.0f, 50.0f));
        optionsButton->OnPressed = []() {
            Logger::Info("Options!");
        };
        vbox->AddChild(optionsButton);

        auto mainMenuButton = std::make_shared<Button>("MainMenuButton");
        mainMenuButton->SetText("MAIN MENU");
        mainMenuButton->SetSize(Vec2(280.0f, 50.0f));
        mainMenuButton->OnPressed = [this]() {
            Logger::Info("Return to main menu!");
            SetUIState(UIState::MainMenu);
        };
        vbox->AddChild(mainMenuButton);

        UISystem::SetRoot(root);
    }
}