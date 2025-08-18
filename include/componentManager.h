#ifndef SASAKALI_ECS_COMPONENT_MANAGER_H
#define SASAKALI_ECS_COMPONENT_MANAGER_H

/**
 * @file componentManager.h
 * @brief Header file that contains ComponentManager class and its functionalities
 *
 * Overview:
 *
 * ComponentManager class is an interface to manage different component type pools. For every
 * types of component, there should be only one pool for each of them. In order to do it, an ID
 * is given to each type of compoenent. The ComponentManager class is consist of two
 * containers: componentPools and maskMap.
 *
 * componentPools (std::vector<std::unique_ptr<IComponentPoo>>) is an array of pointers that
 * holds the addresses to different component type pool.
 *
 * maskMap (std::vector<EntityMask>) is a map from a unique component type IDs to its mask that will
 * be used by Entity.
 *
 *
 * Unique Component Type ID :
 *
 * A unique component type ID is required for ComponentManager to ensure there is only one pool for
 * every component type. Therefore, a mechanism to assign an ID to a specific component type is also
 * needed for ComponentManager to works properly.
 *
 * A mechanism that this implementation is using works by mapping a type index to the ID to enable fast
 * lookup when a component type is already registered and given an ID.
 */

#include <cstddef>
#include <limits>
#include <typeindex>
#include <memory>
#include <vector>
#include <unordered_map>
#include "types.h"
#include "componentPool.h"

namespace ecs {

/**
 * @class ComponentManager componentManager.h "include/componentManager.h"
 * @brief A class that acts as an interface to manage different component type pools
 */
class ComponentManager {
public:
    /****************************************************
     * @name Special Member Functions
     ****************************************************/
    /// @{

    /**
     * @brief ComponentManager class' default constructor
     *
     * Set maxID & nextID to the desired or the default value and initializing
     * idMap, componentPools, poolMap, and maskMap
     *
     * @param maxid Maximum number of component types. Default: std::numeric_limits<ComponetTypeID>::max()
     * @param nextid The starting ID value for the first component type. Default: 0
     */
    ComponentManager(ComponentTypeID maxid = std::numeric_limits<ComponentTypeID>::max(), ComponentTypeID nextid = 0);

    /// @}

    /****************************************************
     * @name Capacity Functions
     ****************************************************/
    /// @{

    /**
     * @brief Check whether componentPools is empty
     * @return bool - true if componetPools is empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Returns the number of elements in componentPools
     * @return std::size_t - the number of elements in componentPools
     */
    std::size_t size() const;

    /// @}

    /****************************************************
     * @name Component Type Management
     ****************************************************/
    /// @{

    /**
     * @brief Check whether a component type has been registered
     * @return bool - true if a component type has been registered, false otherwise
     */
    template<typename T>
    bool isComponentTypeRegistered() const;

    /**
     * @brief Register a component type to the ComponentManager
     *
     * Component type registration is done by assigning a unique ID and mask for it
     * then adding it to the componentPools.
     *
     * @throw std::runtime_error if component type is already registered
     */
    template<typename T>
    void registerComponentType();

    /**
     * @brief Returns a mask corresponding to a specific component type
     * @return EntityMask - A mask corresponding to a specific component type
     * @throw std::runtime_error if requested component type has not been registered
     */
    template<typename T>
    EntityMask getComponentTypeMask() const;

    /// @}

    /****************************************************
     * @name Component Data Operations
     ****************************************************/
    /// @{

    /**
     * @brief Check whether an entity has a component of a specific component type
     * @param id Entity's ID for the checked entity
     * @return bool - true if the entity has a component of the component type
     */
    template<typename T>
    bool hasComponent(const EntityID id) const;

    /**
     * @brief Return a mutable reference to an entity's component of a specific component type
     * @param id Entity's ID for the accessed entity
     * @return T& - A mutable reference to the entity's component of the component type
     * @throw std::runtime_error if requested component type has not been registered
     */
    template<typename T>
    T& getComponent(const EntityID id);

    /**
     * @brief Return a read-only reference to an entity's component of a specific component type
     * @param id Entity's ID for the accessed entity
     * @return const T& - A read-only reference to the entity's component of the component type
     * @throw std::runtime_error if requested component type has not been registered
     */
    template<typename T>
    const T& getComponent(const EntityID id) const;

    /**
     * @brief Assign a component of a specific component type to an entity
     * @param id Entity's ID for the accessed entity
     * @param component New component of a specific component type for the entity
     * @throw std::runtime_error if requested component type has not been registered
     */
    template<typename T>
    void assignComponent(const EntityID id, const T component);

    /**
     * @brief Unassign a component of a specific component type from an entity
     * @param id Entity's ID for the removed entity
     * @throw std::runtime_error if requested component type has not been registered
     */
    template<typename T>
    void unassignComponent(const EntityID id);

    /// @}

    /****************************************************
     * @name System-Level Operations
     ****************************************************/
    /// @{

    /**
     * @brief Notify all component pools that an entity has been destroyed
     * @param id Destroyed entity's id
     */
    void entityDestroyed(const EntityID id);

    /// @}

private:
    /**
     * @brief ComponentManager's private member function to assign a unique ID to a specific component type
     * @return ComponentTypeID - A unique ID for a specific component type
     * @throw std::runtime_error if the number of registered component types has reached its limit
     */
    template<typename T>
    ComponentTypeID getComponentTypeID();

    /**
     * @brief ComponentManager's private member function to get safely-casted pointer to a component type's pool
     * @return ComponentPool<T>* - A safely-casted pointer to component type's pools
     * @throw std::runtime_error if requested component type has not been registered
     */
    template<typename T>
    ComponentPool<T>* getPool() const;

private:
    ComponentTypeID maxID;
    ComponentTypeID nextID;

    std::unordered_map<std::type_index, ComponentTypeID> idMap;
    std::vector<EntityMask> maskMap;

    std::vector<std::unique_ptr<IComponentPool>> componentPools;
};

} // namespace ecs

#include "componentManager.tpp"

#endif // SASAKALI_ECS_COMPONENT_MANAGER_H