#ifndef ASHEN_PROPERTY_H
#define ASHEN_PROPERTY_H

#include <any>
#include <functional>
#include <utility>

#include "Ashen/Core/Types.h"

namespace ash {
    class Property {
    public:
        using Setter = std::function<void(std::any)>;
        using Getter = std::function<std::any()>;

        // Constructeur par défaut (nécessaire pour std::map)
        Property() : name(""), setter(nullptr), getter(nullptr) {}

        // Constructeur paramétrisé
        Property(String n, Setter s, Getter g)
            : name(std::move(n)), setter(std::move(s)), getter(std::move(g)) {}

        String name;
        Setter setter;
        Getter getter;

        void SetValue(const std::any& v) const {
            if (setter) setter(v);
        }

        [[nodiscard]] std::any GetValue() const {
            return getter ? getter() : std::any();
        }
    };

    class PropertyRegistry {
    public:
        void Register(const Property& prop) {
            properties[prop.name] = prop;
        }

        Property* Get(const String& name) {
            auto it = properties.find(name);
            return it != properties.end() ? &it->second : nullptr;
        }

        [[nodiscard]] const auto& GetAll() const {
            return properties;
        }

    private:
        std::map<String, Property> properties;
    };
}

#endif //ASHEN_PROPERTY_H