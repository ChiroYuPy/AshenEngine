#ifndef VOXELITY_ENTITYMANAGER_H
#define VOXELITY_ENTITYMANAGER_H

#include <vector>
#include <memory>
#include <algorithm>

#include "Ashen/core/Types.h"

#include "Voxelity/entities/Entity.h"

namespace voxelity {
    class World;

    class EntityManager {
    public:
        EntityManager() = default;

        ~EntityManager() = default;

        // Ajouter une entité
        template<typename T, typename... Args>
        T *createEntity(Args &&... args) {
            static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");
            auto entity = pixl::MakeScope<T>(std::forward<Args>(args)...);
            T *ptr = entity.get();
            m_entities.push_back(std::move(entity));
            return ptr;
        }

        // Supprimer une entité
        void removeEntity(Entity *entity) {
            std::erase_if(m_entities, [entity](const std::unique_ptr<Entity> &e) { return e.get() == entity; });
        }

        // Mettre à jour toutes les entités
        void updateAll(const float deltaTime, const World &world) {
            // Supprimer les entités inactives
            std::erase_if(m_entities, [](const std::unique_ptr<Entity> &e) { return !e->isActive; });

            // Mettre à jour les entités actives
            for (const auto &entity: m_entities)
                if (entity->isActive)
                    entity->update(deltaTime, world);
        }

        // Accès aux entités
        const std::vector<pixl::Scope<Entity> > &getEntities() const { return m_entities; }

        size_t getEntityCount() const { return m_entities.size(); }

        // Trouver une entité par type
        template<typename T>
        T *findEntity() {
            static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");
            for (auto &entity: m_entities)
                if (T *typed = dynamic_cast<T *>(entity.get()))
                    return typed;

            return nullptr;
        }

        // Trouver toutes les entités d'un type
        template<typename T>
        std::vector<T *> findAllEntities() {
            static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");
            std::vector<T *> result;
            for (auto &entity: m_entities)
                if (T *typed = dynamic_cast<T *>(entity.get()))
                    result.push_back(typed);

            return result;
        }

        void clear() {
            m_entities.clear();
        }

    private:
        std::vector<pixl::Scope<Entity> > m_entities;
    };
}

#endif //VOXELITY_ENTITYMANAGER_H
