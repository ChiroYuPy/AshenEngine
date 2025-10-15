#ifndef ASHEN_ICONFIG_H
#define ASHEN_ICONFIG_H

#include <string>

#include "Ashen/Core/Types.h"

namespace ash {
    struct IConfig {
        virtual ~IConfig() = default;

        virtual bool HasKey(const std::string& key) const = 0;

        virtual std::string GetString(const std::string& key, const std::string& defaultValue = "") const = 0;
        virtual int GetInt(const std::string& key, int defaultValue = 0) const = 0;
        virtual float GetFloat(const std::string& key, float defaultValue = 0.f) const = 0;
        virtual bool GetBool(const std::string& key, bool defaultValue = false) const = 0;

        virtual Vector<std::string> GetStringArray(const std::string& key) const = 0;
        virtual Vector<int> GetIntArray(const std::string& key) const = 0;
        virtual Vector<float> GetFloatArray(const std::string& key) const = 0;
        virtual Vector<bool> GetBoolArray(const std::string& key) const = 0;
    };
}

#endif //ASHEN_ICONFIG_H