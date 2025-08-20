/**
 * @file componentManager.cpp
 * @brief Definitions and implementations of non-templated functions in ComponentManager class
 */

#include <cstddef>
#include "types.h"
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

// Notify all component pools that an entity has been destroyed
void ComponentManager::entityDestroyed(const EntityID id) {
    for (const auto& genericPool : componentPools) {
        const auto& pool { genericPool.get() };

        pool->unassignComponent(id);
    }
}

} // namespace ecs