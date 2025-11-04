#include "Ashen/Scene/SceneSerializer.h"
#include "Ashen/Core/Logger.h"
#include "Ashen/Nodes/Node2D.h"
#include "Ashen/Nodes/Node3D.h"
#include "Ashen/Utils/FileSystem.h"

#include <fstream>

namespace ash {
    // ========== Helper Functions ==========

    namespace {
        // Serialize Vec2
        json SerializeVec2(const Vec2& v) {
            return json{{"x", v.x}, {"y", v.y}};
        }

        // Deserialize Vec2
        Vec2 DeserializeVec2(const json& j) {
            return Vec2(j["x"], j["y"]);
        }

        // Serialize Vec3
        json SerializeVec3(const Vec3& v) {
            return json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
        }

        // Deserialize Vec3
        Vec3 DeserializeVec3(const json& j) {
            return Vec3(j["x"], j["y"], j["z"]);
        }

        // Serialize Quaternion
        json SerializeQuat(const Quaternion& q) {
            return json{{"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w}};
        }

        // Deserialize Quaternion
        Quaternion DeserializeQuat(const json& j) {
            return Quaternion(j["w"], j["x"], j["y"], j["z"]);
        }

        // Serialize Transform2D
        json SerializeTransform2D(const Transform2D& t) {
            json j;
            j["position"] = SerializeVec2(t.position);
            j["rotation"] = t.rotation;
            j["scale"] = SerializeVec2(t.scale);
            return j;
        }

        // Deserialize Transform2D
        Transform2D DeserializeTransform2D(const json& j) {
            Transform2D t;
            t.position = DeserializeVec2(j["position"]);
            t.rotation = j["rotation"];
            t.scale = DeserializeVec2(j["scale"]);
            return t;
        }

        // Serialize Transform3D
        json SerializeTransform3D(const Transform3D& t) {
            json j;
            j["position"] = SerializeVec3(t.position);
            j["rotation"] = SerializeQuat(t.rotation);
            j["scale"] = SerializeVec3(t.scale);
            return j;
        }

        // Deserialize Transform3D
        Transform3D DeserializeTransform3D(const json& j) {
            Transform3D t;
            t.position = DeserializeVec3(j["position"]);
            t.rotation = DeserializeQuat(j["rotation"]);
            t.scale = DeserializeVec3(j["scale"]);
            return t;
        }

        // Get node type string
        String GetNodeType(const Node* node) {
            if (dynamic_cast<const Node3D*>(node)) {
                return "Node3D";
            }
            if (dynamic_cast<const Node2D*>(node)) {
                return "Node2D";
            }
            return "Node";
        }

        // Create node from type string
        Own<Node> CreateNodeFromType(const String& type, const String& name) {
            if (type == "Node3D") {
                return MakeOwn<Node3D>(name);
            }
            if (type == "Node2D") {
                return MakeOwn<Node2D>(name);
            }
            return MakeOwn<Node>(name);
        }
    }

    // ========== SceneSerializer Implementation ==========

    bool SceneSerializer::SaveToFile(const NodeGraph& graph, const fs::path& filepath) {
        try {
            json sceneJson = Serialize(graph);

            std::ofstream file(filepath);
            if (!file.is_open()) {
                Logger::Error() << "Failed to open file for writing: " << filepath;
                return false;
            }

            file << sceneJson.dump(2); // Pretty print with 2-space indentation
            file.close();

            Logger::Info() << "Scene saved to: " << filepath;
            return true;
        } catch (const std::exception& e) {
            Logger::Error() << "Failed to save scene: " << e.what();
            return false;
        }
    }

    Own<Node> SceneSerializer::LoadFromFile(const fs::path& filepath) {
        try {
            if (!FileSystem::Exists(filepath)) {
                Logger::Error() << "Scene file not found: " << filepath;
                return nullptr;
            }

            std::ifstream file(filepath);
            if (!file.is_open()) {
                Logger::Error() << "Failed to open file for reading: " << filepath;
                return nullptr;
            }

            json sceneJson;
            file >> sceneJson;
            file.close();

            auto root = Deserialize(sceneJson);

            if (root) {
                Logger::Info() << "Scene loaded from: " << filepath;
            } else {
                Logger::Error() << "Failed to deserialize scene from: " << filepath;
            }

            return root;
        } catch (const std::exception& e) {
            Logger::Error() << "Failed to load scene: " << e.what();
            return nullptr;
        }
    }

    json SceneSerializer::Serialize(const NodeGraph& graph) {
        json sceneJson;
        sceneJson["version"] = "1.0";
        sceneJson["engine"] = "AshenEngine";

        if (auto root = graph.GetRoot()) {
            sceneJson["root"] = SerializeNode(root);
        } else {
            sceneJson["root"] = nullptr;
        }

        return sceneJson;
    }

    Own<Node> SceneSerializer::Deserialize(const json& j) {
        if (!j.contains("root") || j["root"].is_null()) {
            Logger::Warn("Scene has no root node");
            return nullptr;
        }

        return DeserializeNode(j["root"]);
    }

    json SceneSerializer::SerializeNode(const Node* node) {
        if (!node) {
            return nullptr;
        }

        json nodeJson;

        // Basic properties
        nodeJson["type"] = GetNodeType(node);
        nodeJson["name"] = node->GetName();
        nodeJson["process_enabled"] = node->IsProcessing();

        // Groups
        json groupsJson = json::array();
        for (const auto& group : node->GetGroups()) {
            groupsJson.push_back(group);
        }
        nodeJson["groups"] = groupsJson;

        // Type-specific properties
        if (auto node2D = dynamic_cast<const Node2D*>(node)) {
            nodeJson["transform"] = SerializeTransform2D(node2D->local_transform);
        } else if (auto node3D = dynamic_cast<const Node3D*>(node)) {
            nodeJson["transform"] = SerializeTransform3D(node3D->local_transform);
        }

        // Children
        json childrenJson = json::array();
        for (size_t i = 0; i < node->GetChildCount(); ++i) {
            if (auto child = node->GetChild(i)) {
                childrenJson.push_back(SerializeNode(child));
            }
        }
        nodeJson["children"] = childrenJson;

        return nodeJson;
    }

    Own<Node> SceneSerializer::DeserializeNode(const json& j) {
        if (j.is_null()) {
            return nullptr;
        }

        try {
            // Get node type and name
            String type = j.value("type", "Node");
            String name = j.value("name", "Node");

            // Create node of appropriate type
            auto node = CreateNodeFromType(type, name);

            // Set basic properties
            if (j.contains("process_enabled")) {
                node->SetProcessMode(j["process_enabled"]);
            }

            // Set groups
            if (j.contains("groups") && j["groups"].is_array()) {
                for (const auto& group : j["groups"]) {
                    node->AddToGroup(group);
                }
            }

            // Set type-specific properties
            if (j.contains("transform")) {
                if (auto node2D = dynamic_cast<Node2D*>(node.get())) {
                    node2D->local_transform = DeserializeTransform2D(j["transform"]);
                } else if (auto node3D = dynamic_cast<Node3D*>(node.get())) {
                    node3D->local_transform = DeserializeTransform3D(j["transform"]);
                }
            }

            // Deserialize children
            if (j.contains("children") && j["children"].is_array()) {
                for (const auto& childJson : j["children"]) {
                    if (auto child = DeserializeNode(childJson)) {
                        node->AddChild(std::move(child));
                    }
                }
            }

            return node;
        } catch (const std::exception& e) {
            Logger::Error() << "Failed to deserialize node: " << e.what();
            return nullptr;
        }
    }
}
