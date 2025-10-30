#ifndef ASHEN_ICONFIG_H
#define ASHEN_ICONFIG_H

#include "Ashen/Core/Types.h"

namespace ash {
    class IConfig {
    public:
        virtual ~IConfig() = default;

        virtual bool HasKey(const String &key) const = 0;

        virtual String GetString(const String &key, const String &defaultValue = "") const = 0;

        virtual int GetInt(const String &key, int defaultValue = 0) const = 0;

        virtual float GetFloat(const String &key, float defaultValue = 0.f) const = 0;

        virtual bool GetBool(const String &key, bool defaultValue = false) const = 0;

        virtual Vector<String> GetStringArray(const String &key) const = 0;

        virtual Vector<int> GetIntArray(const String &key) const = 0;

        virtual Vector<float> GetFloatArray(const String &key) const = 0;

        virtual Vector<bool> GetBoolArray(const String &key) const = 0;
    };
}

#endif //ASHEN_ICONFIG_H