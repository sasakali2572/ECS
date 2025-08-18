/**
 * @file componentPool.tpp
 * @brief Definitions and implementations of ComponentPool templated class
 */

#include <cstddef>
#include <stdexcept>
#include "types.h"
#include "componentPool.h"

namespace ecs {

// ComponentPool default constructor, initialize the members
template<typename T>
ComponentPool<T>::ComponentPool() :
    sparseMap(),
    denseMap(),
    densePool()
{}

// Check whether densePool is empty
template<typename T>
bool ComponentPool<T>::empty() const {
    return densePool.empty();
}

// Returns the number of elements in densePool
template<typename T>
std::size_t ComponentPool<T>::size() const {
    return densePool.size();
}

// Check whether an entity has a component in the pool
template<typename T>
bool ComponentPool<T>::hasComponent(const EntityID id) const {
    if (id < sparseMap.size()) {
        return sparseMap.at(id) != NULL_COMPONENT_INDEX;

    } else {
        return false;
    }
}

// Return a writable reference to an entity's component
template<typename T>
T& ComponentPool<T>::getComponent(const EntityID id) {
    if (hasComponent(id)) {
        return densePool.at(sparseMap.at(id));

    } else {
        throw std::runtime_error("ecs::ComponetPool::getComponent() - Requested entity does not have a component of this type");
    }
}

// Return a read-only reference to an entity's component
template<typename T>
const T& ComponentPool<T>::getComponent(const EntityID id) const {
    if (hasComponent(id)) {
        return densePool.at(sparseMap.at(id));

    } else {
        throw std::runtime_error("ecs::ComponetPool::getComponent() - Requested entity does not have a component of this type");
    }
}
// Assign a component to an entity
template<typename T>
void ComponentPool<T>::assignComponent(const EntityID id, const T component) {
    // Resize sparseMap if the entity's ID is not yet listed in it
    if (id >= sparseMap.size()) {
        sparseMap.resize(id + 1, NULL_COMPONENT_INDEX);
    }

    // If the entity already has a component, set it to the new value, else add the component
    // to the back of the densePool.
    if (hasComponent(id)) {
        densePool.at(sparseMap.at(id)) = component;

    } else {
        densePool.push_back(component);
        denseMap.push_back(id);

        sparseMap.at(id) = densePool.size() - 1;
    }
}

// Unassign a component of an entity
template<typename T>
void ComponentPool<T>::unassignComponent(const EntityID id) {
    if (hasComponent(id)) {
        const ComponentIndex indexToRemove { sparseMap.at(id) };
        const ComponentIndex lastIndex { densePool.size() - 1 };
        const EntityID lastIndexID { denseMap.at(lastIndex) };

        // Swapping the value of denseMap and densePool at the index to remove to the value of the last element
        denseMap.at(indexToRemove) = lastIndexID;
        densePool.at(indexToRemove) = densePool.at(lastIndex);

        // Popping the last index from denseMap and densePool
        denseMap.pop_back();
        densePool.pop_back();

        // Updating the sparseMap
        sparseMap.at(lastIndexID) = indexToRemove;
        sparseMap.at(id) = NULL_COMPONENT_INDEX;

    }
}

} // namespace ecs