#include "Voxelity/layers/DebugLayer.h"

#include "Voxelity/VoxelityApp.h"
#include "Ashen/core/Window.h"

voxelity::DebugLayer::DebugLayer() {
    m_DebugCamera = std::make_shared<ash::OrthographicCamera>();
}

void voxelity::DebugLayer::OnRender() {
}

void voxelity::DebugLayer::OnEvent(ash::Event &event) {
}