#ifndef ASHEN_ENGINE_H
#define ASHEN_ENGINE_H

// AshenEngine - Complete Game Engine

// Core Systems
#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Logger.h"
#include "Core/Layer.h"
#include "Core/Types.h"
#include "Core/Window.h"

// Math
#include "Math/Math.h"
#include "Math/Transform.h"
#include "Math/BBox.h"
#include "Math/Random.h"

// Events
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

// Graphics API
#include "GraphicsAPI/Shader.h"
#include "GraphicsAPI/Texture.h"
#include "GraphicsAPI/Buffer.h"
#include "GraphicsAPI/VertexArray.h"
#include "GraphicsAPI/FrameBuffer.h"
#include "GraphicsAPI/RenderCommand.h"

// Graphics
#include "Graphics/Cameras/Camera.h"
#include "Graphics/CameraControllers/FPSCameraController.h"
#include "Graphics/CameraControllers/OrbitCameraController.h"
#include "Graphics/Objects/Mesh.h"
#include "Graphics/Objects/Material.h"
#include "Graphics/Rendering/Renderer.h"
#include "Graphics/Rendering/Renderer2D.h"
#include "Graphics/Rendering/Renderer3D.h"

// Post-Processing
#include "Graphics/PostProcessing/PostProcessing.h"

// Nodes & Scene
#include "Nodes/Node.h"
#include "Nodes/Node2D.h"
#include "Nodes/Node3D.h"
#include "Nodes/NodeGraph.h"
#include "Nodes/Control.h"
#include "Scene/SceneSerializer.h"

// Physics (NEW!)
#include "Physics/Physics.h"

// Animation (NEW!)
#include "Animation/Animation.h"

// Particles (NEW!)
#include "Particles/Particles.h"

// UI (NEW!)
#include "UI/UI.h"

// Audio
#include "Audio/AudioManager.h"
#include "Audio/AudioSource.h"

// Resources
#include "Resources/ResourceManager.h"

// Utilities
#include "Utils/Profiler.h"
#include "Utils/FileSystem.h"
#include "Utils/ImageLoader.h"

// ImGui
#include "ImGui/ImGuiLayer.h"
#include "ImGui/SceneHierarchyPanel.h"
#include "ImGui/InspectorPanel.h"
#include "ImGui/PerformanceMonitorPanel.h"

#endif // ASHEN_ENGINE_H