/**
 * @file scene.tpp
 * @brief Definitions and implementation of Scene class
 */

#include <stdexcept>
#include <memory>
#include "types.h"
#include "scene.h"

namespace ecs {

Scene::Scene() :
    entityManager(std::make_unique<EntityManager>()),
    componentManager(std::make_unique<ComponentManager>())
{}

bool Scene::isEntityValid(const Entity& entity) const {
    return entityManager->isValid(entity);
}

EntityMask Scene::getMask(const Entity& entity) const {
    if (isEntityValid(entity)) {
        return entityManager->getMask(entity);
    } else {
        throw std::runtime_error("ecs::Scene::getMask() - Requested entity is not a valid entity");
    }
}

Entity Scene::createEntity() {
    return entityManager->createEntity(0);
}

void Scene::destroyEntity(const Entity& entity) {
    if (isEntityValid(entity)) {
        entityManager->destroyEntity(entity);
        componentManager->entityDestroyed(entity.id);

    } else {
        throw std::runtime_error("ecs::Scene::destroyEntity() - Requested entity is not a valid entity");
    }
}

template<typename T>
void Scene::registerComponent() {
    componentManager->registerComponentType<T>();
}

template<typename T>
bool Scene::hasComponent(const Entity& entity) const {
    if (isEntityValid(entity)) {
        return hasComponents<T>(entity);

    } else {
        throw std::runtime_error("ecs::Scene::hasComponent() - Requested entity is not a valid entity");
    }
}

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

template<typename T>
T& Scene::getComponent(const Entity& entity) {
    if (isEntityValid(entity)) {
        return componentManager->getComponent<T>(entity.id);

    } else {
        throw std::runtime_error("ecs::Scene::getComponent() - Requested entity is not a valid entity");
    }
}

template <typename T>
const T& Scene::getComponent(const Entity& entity) const {
    if (isEntityValid(entity)) {
        return componentManager->getComponent<T>(entity.id);

    } else {
        throw std::runtime_error("ecs::Scene::getComponent() - Requested entity is not a valid entity");
    }
}

template<typename T>
void Scene::addComponent(const Entity& entity, T component) {
    if (isEntityValid(entity)) {
        componentManager->assignComponent<T>(entity.id, component);
        entityManager->addMask(entity, componentManager->getComponentTypeMask<T>());

    } else {
        throw std::runtime_error("ecs::Scene::addComponent() - Requested entity is not a valid entity");
    }
}

template<typename T>
void Scene::removeComponent(const Entity& entity) {
    if (isEntityValid(entity)) {
        componentManager->unassignComponent<T>(entity.id);
        entityManager->removeMask(entity, componentManager->getComponentTypeMask<T>());

    } else {
        throw std::runtime_error("ecs::Scene::removeComponent() - Requested entity is not a valid entity");
    }
}

const EntityManager& Scene::getEntityManager() const {
    return *entityManager;
}

const ComponentManager& Scene::getComponentManager() const {
    return *componentManager;
}

} // namespace ecs