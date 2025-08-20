/**
 * @file componentManager.tpp
 * @brief Definitions and implementations of ComponentManager class
 */

#include <cstddef>
#include <stdexcept>
#include <typeindex>
#include <memory>
#include "types.h"
#include "componentPool.h"
#include "componentManager.h"

namespace ecs {

// ComponentManager class' default constructor
ComponentManager::ComponentManager(ComponentTypeID maxid, ComponentTypeID nextid) :
    maxID(maxid),
    nextID(nextid),
    idMap(),
    maskMap(),
    componentPools()
{}

// Check whether componentPools is empty
bool ComponentManager::empty() const {
    return componentPools.empty();
}

// Returns the number of elements in componentPools
std::size_t ComponentManager::size() const {
    return componentPools.size();
}

// Check whether a component type has been registered
template<typename T>
bool ComponentManager::isComponentTypeRegistered() const {
    const std::type_index typeIndex { std::type_index(typeid(T)) };

    return idMap.contains(typeIndex);
}

// Register a component type to the ComponentManager
template<typename T>
void ComponentManager::registerComponentType() {
    // Register only componentType that not already registered
    if (!isComponentTypeRegistered<T>()) {
        const ComponentTypeID typeID { getComponentTypeID<T>() };

        auto newPool { std::make_unique<ComponentPool<T>>() };

        if (typeID >= componentPools.size()) {
            maskMap.resize(typeID + 1);
            componentPools.resize(typeID + 1);
        }
        maskMap.at(typeID) = (1ULL << typeID);
        componentPools.at(typeID) = std::move(newPool);

    } else {
        throw std::runtime_error("ecs::ComponentManager::registerComponentType() - Can't register same component type more than once");
    }
}

// Returns a mask corresponding to a specific component type
template<typename T>
EntityMask ComponentManager::getComponentTypeMask() const {
    if (isComponentTypeRegistered<T>()) {
        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const ComponentTypeID typeID { idMap.at(typeIndex) };

        return maskMap.at(typeID);

    } else {
        throw std::runtime_error("ecs::ComponentManager::getComponentTypeMask() - Component type is not registered");
    }
}

// Check whether an entity has a component of a specific component type
template<typename T>
bool ComponentManager::hasComponent(const EntityID id) const {
    if (isComponentTypeRegistered<T>()) {
        auto pool { getPool<T>() };

        return pool->hasComponent(id);

    } else {
        return false;
    }
}

// Return a mutable reference to an entity's component of a specific component type
template<typename T>
T& ComponentManager::getComponent(const EntityID id) {
    if (isComponentTypeRegistered<T>()) {
        auto pool { getPool<T>() };

        return pool->getComponent(id);

    } else {
        throw std::runtime_error("ecs::ComponentManager::getComponent() - Component type is not registered");
    }
}

// Return a read-only reference to an entity's component of a specific component type
template<typename T>
const T& ComponentManager::getComponent(const EntityID id) const {
    if (isComponentTypeRegistered<T>()) {
        auto pool { getPool<T>() };

        return pool->getComponent(id);

    } else {
        throw std::runtime_error("ecs::ComponentManager::getComponent() - Component type is not registered");
    }
}

// Assign a component of a specific component type to an entity
template<typename T>
void ComponentManager::assignComponent(const EntityID id, const T component) {
    if (isComponentTypeRegistered<T>()) {
        auto pool { getPool<T>() };

        pool->assignComponent(id, component);

    } else {
        throw std::runtime_error("ecs::ComponentManager::assignComponent() - Component type is not registered");
    }
}

// Unassign a component of a specific component type from an entity
template<typename T>
void ComponentManager::unassignComponent(const EntityID id) {
    if (isComponentTypeRegistered<T>()) {
        auto pool { getPool<T>() };

        pool->unassignComponent(id);

    } else {
        throw std::runtime_error("ecs::ComponentManager::unassignComponent() - Component type is not registered");
    }
}

// Notify all component pools that an entity has been destroyed
void ComponentManager::entityDestroyed(const EntityID id) {
    for (const auto& genericPool : componentPools) {
        const auto& pool { genericPool.get() };

        pool->unassignComponent(id);
    }
}

// ComponentManager's private member function to assign a unique ID to a specific component type
template<typename T>
ComponentTypeID ComponentManager::getComponentTypeID() {
    const std::type_index typeIndex { std::type_index(typeid(T)) };

    if (idMap.contains(typeIndex)) {
        return idMap.at(typeIndex);

    } else {
        if (nextID < maxID) {
            const ComponentTypeID newID { nextID++ };

            idMap.insert({ typeIndex, newID });

            return newID;

        } else {
            throw std::runtime_error("ecs::ComponentManager::getComponentTypeID() - Number of registered component type has reached its limit, failed to assign new ID");
        }
    }
}

// ComponentManager's private member function to get safely-casted pointer to a component type's pool
template<typename T>
ComponentPool<T>* ComponentManager::getPool() const {
    if (isComponentTypeRegistered<T>()) {

        const std::type_index typeIndex { std::type_index(typeid(T)) };
        const ComponentTypeID typeID { idMap.at(typeIndex) };

        const auto genericPool { componentPools.at(typeID).get() };
        return static_cast<ComponentPool<T>*>(genericPool);

    } else {
        throw std::runtime_error("ecs::ComponentManager::getPool() - Component type is not registered");
    }
}

} // namespace ecs