#include "Voxelity/layers/DebugLayer.h"

#include "Voxelity/VoxelityApp.h"
#include "Ashen/core/Window.h"

voxelity::DebugLayer::DebugLayer() {
    m_DebugCamera = std::make_shared<pixl::OrthographicCamera>();
    const pixl::Window &window = VoxelityApp::Get().GetWindow();
    m_DebugCamera->SetBounds(0.0f, static_cast<float>(window.GetWidth()),
                              0.0f, static_cast<float>(window.GetHeight()));
}

void voxelity::DebugLayer::OnRender() {

}

void voxelity::DebugLayer::OnEvent(pixl::Event &event) {

}
