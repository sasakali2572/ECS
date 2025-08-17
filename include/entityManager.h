#ifndef SASAKALI_ECS_ENTITY_MANAGER_H
#define SASAKALI_ECS_ENTITY_MANAGER_H

/**
 * @file entityManager.h
 * @brief Header file that contains EntityManager class and its functionalities
 *
 * Overview:
 *
 * EntityManager class is an isolated interface for the entity functionalities.
 *
 * Entity is just an identifier that may be associated to components. It is implemented to be
 * comprised of an ID (EntityID), a generation number (EntityGen), anda mask(EntityMask).
 *
 * Entity's ID is a plain integer that generally used as an index to access everything related
 * to it. Entity's generation number is a counter for a specific ID that holds an information
 * about how many time the ID has been recycled. It is used together with the ID as an
 * additional identification parameter of an entity in order to be able to access or modify
 * the entity or its component safely. Entity's mask is a string of bit that stores
 * information about which components are associated with the entity.
 *
 *
 * Entity Creation, Destruction, and ID Recycling:
 *
 * An entity is created by simply giving it an integer value as its ID. The ID given to it is
 * obtained from one of the two sources, that is by using a recyled ID that no longer being
 * used by an entity, or by giving it a completely new ID. A mask value of the entity is then
 * being stored in an array with the ID as index to it.
 *
 * An entity is destructed by resetting the mask of the entity which placed in an array at the
 * index of its ID to NULL_MASK and recyling the destructed entity ID to a stack of recyled IDs
 * which will be used when creating a new entity.
 */

#include <cstddef>
#include <limits>
#include <stack>
#include <vector>
#include "types.h"

namespace ecs {

/**
 * @class EntityManager entityManager.h "include/entityManager.h"
 * @brief A class that acts as an isolated interface for the entity functionalities.
 */
class EntityManager {
public:
    /****************************************************
     * @name Special Member Functions
     ****************************************************/
    /// @{

    /**
     * @brief EntityManager class' default constructor
     *
     * Set maxID & newID to the desired or the default value and initializing
     * recyledIDs, entityGens, & entityMasks
     *
     * @param maxid Maximum number of entity. Default: std::numeric_limits<EntityID>::max()
     * @param newid The starting ID value for the first entity. Default: 0
     */
    EntityManager(const EntityID maxid = std::numeric_limits<EntityID>::max(), const EntityID newid = 0);

    /// @}

    /****************************************************
     * @name Capacity Functions
     ****************************************************/
    /// @{

    /**
     * @brief Check whether entityGens is empty
     * @return bool - true if the entityGens is empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Returns the number of elements in entityGens
     * @return std::size_t - the number of elements in entityGens
     */
    std::size_t size() const;

    /// @}

    /****************************************************
     * @name Entity Validity Check
     ****************************************************/
    /// @{

    /**
     * @brief Check whether an entity is valid
     *
     * An valid entity is the one that has matching ID and generation number with
     * a non NULL_MASK mask.
     *
     * @param entity A safe Entity handle with an ID and generation number to check
     * @return bool - true if the entity is valid, false otherwise
     */
    bool isValid(const Entity& entity) const;

    /// @}

    /****************************************************
     * @name Entity's Mask Operations
     ****************************************************/
    /// @{

    /**
     * @brief Returns the mask of an entity
     * @param entity A safe Entity handle to access an entity mask
     * @return EntityMask - the mask of the requested entity
     * @throw std::runtime_error if the requested entity is not valid
     */
    EntityMask getMask(const Entity& entity) const;

    /**
     * @brief Set an entity's mask to a new value
     * @param entity A safe Entity handle to access and modify an entity mask
     * @param mask New value of EntityMask for the entity's mask
     * @throw std::runtime_error if the requested entity is not valid
     */
    void setMask(const Entity& entity, const EntityMask mask);

    /**
     * @brief Activate bit(s) of an entity's mask
     * @param entity A safe Entity handle to access and modify an entity mask
     * @param mask A string of bit(s) representing added components
     * @throw std::runtime_error if the requested entity is not valid
     */
    void addMask(const Entity& entity, const EntityMask mask);

    /**
     * @brief Deactivate bit(s) of an entity's mask
     * @param entity A safe Entity handle to access and modify an entity mask
     * @param mask A string of bit(s) representing removed components
     * @throw std::runtime_error if the requested entity is not valid
     */
    void removeMask(const Entity& entity, const EntityMask mask);

    /// @}

    /****************************************************
     * @name Entity Creation and Destruction
     ****************************************************/
    /// @{

    /**
     * @brief Entity creation
     *
     * Entity creation done by first assigning an ID to the new entity either using
     * recyled ID or a new one. After a new ID is acquired, then set the generation number
     * and mask of the new entity.
     *
     * @param mask Value of EntityMask for the new entity
     * @return Entity - A safe Entity handle of the new entity
     * @throw std::runtime_error if the number of entity has reached its limit
     * @throw std::runtime_error if the new entity is not valid
    */
    Entity createEntity(const EntityMask mask);

    /**
     * @brief Entity destruction
     *
     * Entity destruction done by invalidating the entity's ID by incrementing entity's
     * generation number and setting its mask to NULL_MASK, then adding the destroyed
     * entity's ID to the recycledIDs stack.
     *
     * @param entity A safe Entity handle of the detroyed entity
     * @throw std::runtime_error if the requested entity is not valid
     */
    void destroyEntity(const Entity& entity);

    /// @}

private:
    EntityID maxID;
    EntityID newID;

    std::stack<EntityID> recycledIDs;

    std::vector<EntityGen> entityGens;
    std::vector<EntityMask> entityMasks;
};


} // namespace ecs

#endif // SASAKALI_ECS_ENTITY_MANAGER_H