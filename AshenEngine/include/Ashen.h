#ifndef ASHEN_ASHEN_H
#define ASHEN_ASHEN_H

// Core
#include "Ashen/Core/Application.h"
#include "Ashen/Core/EntryPoint.h"
#include "Ashen/Core/Input.h"
#include "Ashen/Core/KeyCodes.h"
#include "Ashen/Core/MouseCodes.h"
#include "Ashen/Core/Layer.h"
#include "Ashen/Core/LayerStack.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Core/Platform.h"
#include "Ashen/Core/Types.h"
#include "Ashen/Core/Color.h"

// Events
#include "Ashen/Events/Event.h"
#include "Ashen/Events/ApplicationEvent.h"
#include "Ashen/Events/KeyEvent.h"
#include "Ashen/Events/MouseEvent.h"

// Math
#include "Ashen/Math/Math.h"
#include "Ashen/Math/BBox.h"

// Graphics
#include "Ashen/Graphics/Camera/Camera.h"
#include "Ashen/Graphics/Rendering/Renderer.h"
#include "Ashen/Graphics/Rendering/Renderer2D.h"
#include "Ashen/Graphics/Rendering/Renderer3D.h"
#include "Ashen/Graphics/Skybox/ISkybox.h"
#include "Ashen/Graphics/Skybox/SkyboxCubeMap.h"

// Renderer
#include "Ashen/GraphicsAPI/GLEnums.h"
#include "Ashen/GraphicsAPI/GLObject.h"
#include "Ashen/GraphicsAPI/FrameBuffer.h"
#include "Ashen/GraphicsAPI/VertexArray.h"
#include "Ashen/GraphicsAPI/Buffer.h"
#include "Ashen/GraphicsAPI/Shader.h"
#include "Ashen/GraphicsAPI/Texture.h"

// Resources
#include "Ashen/Resources/ResourceManager.h"

// Utils
#include "Ashen/Utils/FileSystem.h"
#include "Ashen/Utils/ImageLoader.h"

#endif //ASHEN_ASHEN_H