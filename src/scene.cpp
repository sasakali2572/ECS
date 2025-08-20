/**
 * @file scene.cpp
 * @brief Definitions and implementations of non-templated functions in Scene class
 */

#include <algorithm>
#include <stdexcept>
#include <typeindex>
#include <memory>
#include "types.h"
#include "scene.h"

namespace ecs {

// Scene class' constructor, initialize entityManager and componentManager
Scene::Scene() :
    indexMap(),
    systems(),
    entityManager(std::make_unique<EntityManager>()),
    componentManager(std::make_unique<ComponentManager>())
{}

// Check whether an entity is a valid entity
bool Scene::isEntityValid(const Entity& entity) const {
    return entityManager->isValid(entity);
}

// Returns an entity's mask
EntityMask Scene::getMask(const Entity& entity) const {
    if (isEntityValid(entity)) {
        return entityManager->getMask(entity);
    } else {
        throw std::runtime_error("ecs::Scene::getMask() - Requested entity is not a valid entity");
    }
}

// Create a new entity and set it mask to NULL_MASK
Entity Scene::createEntity() {
    return entityManager->createEntity(NULL_MASK);
}

// Destroy an entity and remove all its components from their corresponding pools
void Scene::destroyEntity(const Entity& entity) {
    if (isEntityValid(entity)) {
        entityManager->destroyEntity(entity);
        componentManager->entityDestroyed(entity.id);

    } else {
        throw std::runtime_error("ecs::Scene::destroyEntity() - Requested entity is not a valid entity");
    }
}

// Update the state of the scene by instructing each system to update
void Scene::update(float deltaTime) {
    for (const auto& system : systems) {
        if (system->isEnabled()) {
            system->update(*this, deltaTime);
        }
    }
}

// Returns a read-only reference to the entity manager for direct access
const EntityManager& Scene::getEntityManager() const {
    return *entityManager;
}

// Returns a read-only reference to the component manager for direct access
const ComponentManager& Scene::getComponentManager() const {
    return *componentManager;
}

// Sort systems in the Scene by their priority value
void Scene::sortSystems() {
    std::sort(systems.begin(), systems.end(),
        [](const std::unique_ptr<System>& a, const std::unique_ptr<System>& b) {
            return a->getPriority() < b->getPriority();
        }
    );

    rebuildMap();
}

// Rebuild indexMap so it point to the correct system
void Scene::rebuildMap() {
    indexMap.clear();

    for (SystemIndex systemIndex { 0 }; systemIndex < systems.size(); systemIndex++) {
        const auto& genericSystem { systems.at(systemIndex).get() };
        const std::type_index typeIndex { std::type_index(typeid(*genericSystem)) };

        indexMap.insert({ typeIndex, systemIndex });
    }
}

} // namespace ecs