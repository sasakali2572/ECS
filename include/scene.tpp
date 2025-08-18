/**
 * @file scene.tpp
 * @brief Definitions and implementation of Scene class
 */

#include <stdexcept>
#include <memory>
#include "types.h"
#include "scene.h"

namespace ecs {

// Scene class' constructor, initialize entityManager and componentManager
Scene::Scene() :
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

// Returns a read-only reference to the entity manager for direct access
const EntityManager& Scene::getEntityManager() const {
    return *entityManager;
}

// Returns a read-only reference to the component manager for direct access
const ComponentManager& Scene::getComponentManager() const {
    return *componentManager;
}

} // namespace ecs