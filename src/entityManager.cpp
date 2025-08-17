/**
 * @file entityManager.cpp
 * @brief Definitions and implementations of EntityManager class
 */

#include <cstddef>
#include <stdexcept>
#include "types.h"
#include "entityManager.h"

namespace ecs {

// EntityManager class' default constructor
EntityManager::EntityManager(const EntityID maxid, const EntityID newid) :
    maxID(maxid),
    newID(newid),
    recycledIDs(),
    entityGens(),
    entityMasks()
{}

// Check whether entityGens is empty
bool EntityManager::empty() const {
    return entityGens.empty();
}

// Returns the number of elements in entityGens
std::size_t EntityManager::size() const {
    return entityGens.size();
}

// Check whether an entity is active/valid
bool EntityManager::isActive(const Entity& entity) const {
    if (entity.id < entityGens.size()) {
        return (entityGens.at(entity.id) == entity.gen && entityMasks.at(entity.id) != NULL_MASK);

    } else {
        return false;
    }
}

// Returns the mask of an entity
EntityMask EntityManager::getMask(const Entity& entity) const {
    if (isActive(entity)) {
        return entityMasks.at(entity.id);

    } else {
        throw std::runtime_error("EntityManager::getMask() - Requested entity does not exists");
    }
}

//Set an entity's mask to a new value
void EntityManager::setMask(const Entity& entity, const EntityMask mask) {
    if (isActive(entity)) {
        entityMasks.at(entity.id) = mask;

    } else {
        throw std::runtime_error("EntityManager::setMask() - Requested entity does not exists");
    }
}

// Activate bit(s) of an entity's mask
void EntityManager::addMask(const Entity& entity, const EntityMask mask) {
    if (isActive(entity)) {
        const EntityMask currentMask { entityMasks.at(entity.id) };
        entityMasks.at(entity.id) = currentMask | mask;

    } else {
        throw std::runtime_error("EntityManager::addMask() - Requested entity does not exists");
    }
}

// Deactivate bit(s) of an entity's mask
void EntityManager::removeMask(const Entity& entity, const EntityMask mask) {
    if (isActive(entity)) {
        const EntityMask currentMask { entityMasks.at(entity.id) };
        entityMasks.at(entity.id) = currentMask & (~mask);

    } else {
        throw std::runtime_error("EntityManager::removeMask() - Requested entity does not exists");
    }
}

// Entity creation
Entity EntityManager::addEntity(const EntityMask mask) {
    if (newID < maxID) { // If the number of entities doesn't exceed the limit
        Entity entity;

        // Use a recycled ID for the new entity
        if (!recycledIDs.empty()) {
            entity.id = recycledIDs.top();
            recycledIDs.pop();

            entity.gen = entityGens.at(entity.id);

            entityMasks.at(entity.id) = mask;

        // Use a new ID for the new entity
        } else {
            entity.id = newID;
            newID++;

            entity.gen = 0;

            entityGens.push_back(entity.gen);
            entityMasks.push_back(mask);
        }

         // Check if the new entity properly added to the list of entities
        if (isActive(entity)) {
            return entity;

        } else {
            throw std::runtime_error("EntityManager::addEntity() - Failed to create a new entity");
        }

    } else {
        throw std::runtime_error("EntityManager::addEntity() - Number of entities has reached the limit, failed to create a new entity");
    }
}

// Entity removal
void EntityManager::removeEntity(const Entity& entity) {
    if(isActive(entity)) {
        // Invalidate the entity
        entityGens.at(entity.id)++;
        entityMasks.at(entity.id) = NULL_MASK;

        // Putting the removed entity's ID to the recycledIDs stack
        recycledIDs.push(entity.id);

    } else {
        throw std::runtime_error("EntityManager::removeEntity() - Requested entity does not exists");
    }
}

} // namespace ecs