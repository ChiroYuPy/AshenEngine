#ifndef ASHEN_SCENESERIALIZER_H
#define ASHEN_SCENESERIALIZER_H

#include <filesystem>
#include <json.hpp>

#include "Ashen/Core/Types.h"
#include "Ashen/Nodes/Node.h"
#include "Ashen/Nodes/NodeGraph.h"

namespace ash {
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    /**
     * @brief Serializes and deserializes scene node graphs to/from JSON
     */
    class SceneSerializer final {
    public:
        SceneSerializer() = default;
        ~SceneSerializer() = default;

        // No copy
        SceneSerializer(const SceneSerializer&) = delete;
        SceneSerializer& operator=(const SceneSerializer&) = delete;

        /**
         * @brief Save a node graph to a JSON file
         * @param graph The node graph to save
         * @param filepath Path to save the scene file
         * @return true if successful, false otherwise
         */
        static bool SaveToFile(const NodeGraph& graph, const fs::path& filepath);

        /**
         * @brief Load a node graph from a JSON file
         * @param filepath Path to the scene file
         * @return Unique pointer to the loaded root node, or nullptr on failure
         */
        static Own<Node> LoadFromFile(const fs::path& filepath);

        /**
         * @brief Serialize a node graph to JSON
         * @param graph The node graph to serialize
         * @return JSON object representing the scene
         */
        static json Serialize(const NodeGraph& graph);

        /**
         * @brief Deserialize a node graph from JSON
         * @param j JSON object to deserialize from
         * @return Unique pointer to the loaded root node, or nullptr on failure
         */
        static Own<Node> Deserialize(const json& j);

        /**
         * @brief Serialize a single node to JSON
         * @param node The node to serialize
         * @return JSON object representing the node
         */
        static json SerializeNode(const Node* node);

        /**
         * @brief Deserialize a single node from JSON
         * @param j JSON object to deserialize from
         * @return Unique pointer to the loaded node, or nullptr on failure
         */
        static Own<Node> DeserializeNode(const json& j);

    private:
        static void SerializeNodeRecursive(const Node* node, json& j);
        static void DeserializeChildren(Node* parent, const json& j);
    };
}

#endif // ASHEN_SCENESERIALIZER_H
