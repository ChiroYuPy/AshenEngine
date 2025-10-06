#ifndef ASHEN_JSONCONFIG_H
#define ASHEN_JSONCONFIG_H

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

#include <json.hpp>
#include "IConfig.h"

class JsonConfig final : public IConfig {
public:
    explicit JsonConfig(const std::string& filepath) : m_filepath(filepath) {
        LoadFromFile(filepath);
    }

    void Reload() {
        LoadFromFile(m_filepath);
    }

    bool HasKey(const std::string& key) const override {
        return GetJsonNode(key, false) != nullptr;
    }

    std::string GetString(const std::string& key, const std::string& defaultValue = "") const override {
        if (const auto node = GetJsonNode(key)) return node->get<std::string>();
        return defaultValue;
    }

    int GetInt(const std::string& key, const int defaultValue = 0) const override {
        if (const auto node = GetJsonNode(key)) return node->get<int>();
        return defaultValue;
    }

    float GetFloat(const std::string& key, const float defaultValue = 0.f) const override {
        if (const auto node = GetJsonNode(key)) return node->get<float>();
        return defaultValue;
    }

    bool GetBool(const std::string& key, const bool defaultValue = false) const override {
        if (const auto node = GetJsonNode(key)) return node->get<bool>();
        return defaultValue;
    }

    std::vector<std::string> GetStringArray(const std::string& key) const override {
        return GetArray<std::string>(key);
    }

    std::vector<int> GetIntArray(const std::string& key) const override {
        return GetArray<int>(key);
    }

    std::vector<float> GetFloatArray(const std::string& key) const override {
        return GetArray<float>(key);
    }

    std::vector<bool> GetBoolArray(const std::string& key) const override {
        return GetArray<bool>(key);
    }

    JsonConfig GetObject(const std::string& key) const {
        if (const auto node = GetJsonNode(key)) {
            JsonConfig sub(*node);
            return sub;
        }
        throw std::runtime_error("JsonConfig: Object key not found: " + key);
    }

    template<typename T>
    T Get(const std::string& key, const T& defaultValue) const {
        if (const auto node = GetJsonNode(key)) return node->get<T>();
        return defaultValue;
    }

private:
    nlohmann::json m_json;
    std::string m_filepath;

    explicit JsonConfig(const nlohmann::json& j) : m_json(j) {}

    void LoadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) throw std::runtime_error("Cannot open JSON file: " + filepath);
        file >> m_json;
    }

    // Dot notation
    const nlohmann::json* GetJsonNode(const std::string& key, const bool throwIfMissing = true) const {
        const nlohmann::json* node = &m_json;
        std::istringstream ss(key);
        std::string token;
        while (std::getline(ss, token, '.')) {
            if (node->contains(token)) {
                node = &(*node)[token];
            } else {
                if (throwIfMissing) throw std::runtime_error("JsonConfig: Key not found: " + key);
                return nullptr;
            }
        }
        return node;
    }

    template<typename T>
    std::vector<T> GetArray(const std::string& key) const {
        std::vector<T> result;
        if (const auto node = GetJsonNode(key, false))
            if (node->is_array())
                for (auto& v : *node) result.push_back(v.get<T>());

        return result;
    }
};

#endif // ASHEN_JSONCONFIG_H
