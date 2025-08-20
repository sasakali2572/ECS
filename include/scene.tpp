/**
 * @file scene.tpp
 * @brief Definitions and implementation of Scene class
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

// Register a new component type to the component manager
template<typename T>
void Scene::registerComponent() {
    componentManager->registerComponentType<T>();
}

// Check whether an entity has a component of a specific type
template<typename T>
bool Scene::hasComponent(const Entity& entity) const {
    if (isEntityValid(entity)) {
        return hasComponents<T>(entity);

    } else {
        throw std::runtime_error("ecs::Scene::hasComponent() - Requested entity is not a valid entity");
    }
}

// Check whether an entity has components of all the specified type
template<typename... ComponentTypes>
bool Scene::hasComponents(const Entity& entity) const {
    if (isEntityValid(entity)) {
        const EntityMask requiredMask { (componentManager->getComponentTypeMask<ComponentTypes>() | ...) };
        const EntityMask entityMask { entityManager->getMask(entity) };

        return (entityMask & requiredMask) == requiredMask;

    } else {
        throw std::runtime_error("ecs::Scene::hasComponents() - Requested entity is not a valid entity");
    }
}

// Return a mutable reference to an entity's component of a specific component type
template<typename T>
T& Scene::getComponent(const Entity& entity) {
    if (isEntityValid(entity)) {
        return componentManager->getComponent<T>(entity.id);

    } else {
        throw std::runtime_error("ecs::Scene::getComponent() - Requested entity is not a valid entity");
    }
}

// Return a read-only reference to an entity's component of a specific component type
template <typename T>
const T& Scene::getComponent(const Entity& entity) const {
    if (isEntityValid(entity)) {
        return componentManager->getComponent<T>(entity.id);

    } else {
        throw std::runtime_error("ecs::Scene::getComponent() - Requested entity is not a valid entity");
    }
}

// Add a component of a specific type to an entity and update its mask
template<typename T>
void Scene::addComponent(const Entity& entity, T component) {
    if (isEntityValid(entity)) {
        componentManager->assignComponent<T>(entity.id, component);
        entityManager->addMask(entity, componentManager->getComponentTypeMask<T>());

    } else {
        throw std::runtime_error("ecs::Scene::addComponent() - Requested entity is not a valid entity");
    }
}

// Remove a component of a specific type from an entity and update its mask
template<typename T>
void Scene::removeComponent(const Entity& entity) {
    if (isEntityValid(entity)) {
        componentManager->unassignComponent<T>(entity.id);
        entityManager->removeMask(entity, componentManager->getComponentTypeMask<T>());

    } else {
        throw std::runtime_error("ecs::Scene::removeComponent() - Requested entity is not a valid entity");
    }
}


// Check whether a system is exists in the Scene
template<typename T>
bool Scene::hasSystem() const {
    const std::type_index typeIndex { std::type_index(typeid(T)) };

    return indexMap.contains(typeIndex);
}

// Returns a mutable reference to a system object
template<typename T>
T& Scene::getSystem() {
    if (hasSystem<T>()) {
        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const SystemIndex systemIndex { indexMap.at(typeIndex) };

        const auto genericSystem { systems.at(systemIndex).get() };
        const auto derivedSystem { dynamic_cast<T*>(genericSystem) };
        return *derivedSystem;

    } else {
        throw std::runtime_error("ecs::Scene::getSystem() - System do not exists in the scene");
    }
}

// Returns a read-only reference to a system object
template<typename T>
const T& Scene::getSystem() const {
    if (hasSystem<T>()) {
        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const SystemIndex systemIndex { indexMap.at(typeIndex) };

        const auto genericSystem { systems.at(systemIndex).get() };
        const auto derivedSystem { dynamic_cast<T*>(genericSystem) };
        return *derivedSystem;

    } else {
        throw std::runtime_error("ecs::Scene::getSystem() - System do not exists in the scene");
    }
}

// add a system to the Scene
template<typename T>
void Scene::addSystem() {
    if (!hasSystem<T>()) {
        systems.push_back(std::make_unique<T>());

        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const SystemIndex systemIndex { systems.size() - 1 };

        indexMap.insert({ typeIndex, systemIndex });

        sortSystems();

    } else {
        throw std::runtime_error("ecs::Scene::addSystem() - Can't add the sam system more than once");
    }
}

// remove a system from the Scene
template<typename T>
void Scene::removeSystem() {
    if (hasSystem<T>()) {
        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const SystemIndex systemIndex { indexMap.at(typeIndex) };

        systems.erase(systems.begin() + systemIndex);

        rebuildMap();

    } else {
        throw std::runtime_error("ecs::Scene::removeSystem() - System do not exists in the scene");
    }
}

// Check whether a system is enabled
template<typename T>
bool Scene::isSystemEnabled() const {
    if (hasSystem<T>()) {
        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const SystemIndex systemIndex { indexMap.at(typeIndex) };

        const auto& system { systems.at(systemIndex) };
        return system->isEnabled();

    } else {
        throw std::runtime_error("ecs::Scene::isSystemEnabled() - System do not exists in the scene");
    }
}

// Set a system to be enabled/disabled
template<typename T>
void Scene::setSystemEnabled(bool isEnabled) {
    if (hasSystem<T>()) {
        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const SystemIndex systemIndex { indexMap.at(typeIndex) };

        const auto& system { systems.at(systemIndex) };
        return system->setEnabled(isEnabled);

    } else {
        throw std::runtime_error("ecs::Scene::isSystemEnabled() - System do not exists in the scene");
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

// Returns an array of Entity handles that have a list of components
template<typename... ComponentTypes>
std::vector<Entity> Scene::getEntitiesWith() const {
    std::vector<Entity> filteredEntities;
    std::vector<Entity> validEntities { entityManager->getValidEntities() };

    const EntityMask requiredMask { (componentManager->getComponentTypeMask<ComponentTypes>() | ...) };

    for (const Entity& entity : validEntities) {
        const EntityMask entityMask { entityManager->getMask(entity) };

        if ((entityMask & requiredMask) == requiredMask) {
            filteredEntities.push_back(entity);
        }
    }

    return filteredEntities;
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