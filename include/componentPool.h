#ifndef SASAKALI_ECS_COMPONENT_POOL_H
#define SASAKALI_ECS_COMPONENT_POOL_H

/**
 * @file componentPool.h
 * @brief Header file that contains ComponentPool class and its functionalities
 *
 * Overview:
 *
 * ComponentPool class is a templated class that provides a component pool to a type of
 * component and acts as an isolated interface to the component pool functionalities.
 *
 * Component pool is a dense array that contains all the components of a specific type and
 * provides cache-friendly data structure. The ComponentPool class consists of three
 * containers: a sparseMap, a denseMap, and a densePool.
 *
 * sparseMap (std::vector<ComponentIndex>) is a map between EntityID as the index to the
 * ComponentIndex which point to the position of the entity's component inside densePool.
 *
 * denseMap (std::vector<EntityID>) is a map that contains the entity's ID associated with
 * a specific component inside the densePool at the same index. This map provides a way to
 * efficiently remove a component from the entity using swap-and-pop operation.
 *
 * densePool (std::vector<T>) with T is the component type is an array of a single type of
 * component that packed densely to be able to used efficiently by the CPU. This array is
 * central for this implementation of ComponentPool class.
 *
 *
 * IComponentPool Base Class:
 *
 * Since ComponentPool is a templated class that is unique for each type of components,
 * IComponentPool acts as the base class for ComponentPool that allow every ComponentPool
 * instances for each component types to be contained in an array as a unique_ptr.
 */

#include <cstddef>
#include <vector>
#include "types.h"

namespace ecs {

/**
 * @class IComponentPool componentPool.h "include/componentPool.h"
 * @brief A base class inherited by ComponentPool that allow every component pool to be
 * stored in a single array as a unique_ptr.
 */
class IComponentPool {
public:
    virtual ~IComponentPool() = default;

    virtual void unassignComponent(const EntityID id) = 0;
};

/**
 * @class ComponentPool componentPool.h "include/componentPool.h"
 * @brief A templated class that acts as an isolated interface for component pool of a
 * specific component type and its functionality.
 */
template<typename T>
class ComponentPool : public IComponentPool {
public:
    /****************************************************
     * @name Special Member Functions
     ****************************************************/
    /// @{

    /**
     * @brief ComponentPool class' default constructor, initialize the members
     */
    ComponentPool();

    /// @}

    /****************************************************
     * @name Capacity Functions
     ****************************************************/
    /// @{

    /**
     * @brief Check whether densePool is empty
     * @return bool - true if densePool is empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Returns the number of elements in densePool
     * @return std::size_t - the number of elements in densePool
     */
    std::size_t size() const;

    /// @}

    /****************************************************
     * @name Component Validity Check
     ****************************************************/
    /// @{

    /**
     * @brief Check whether an entity has a component in the pool
     * @param id Entity's ID for the checked entity
     * @return bool - true if the entity has a component in this pool
     */
    bool hasComponent(const EntityID id) const;

    /// @}

    /****************************************************
     * @name Component Getter Functions
     ****************************************************/
    /// @{

    /**
     * @brief Return a mutable reference to an entity's component
     * @param id Entity's ID for the accessed entity
     * @return T& - A mutable reference to the entity's component
     * @throw std::runtime_error if entity has no component in this pool
     */
    T& getComponent(const EntityID id);

    /**
     * @brief Return a read-only reference to an entity's component
     * @param id Entity's ID for the accessed entity
     * @return const T& - A read-only reference to the entity's component
     * @throw std::runtime_error if entity has no component in this pool
     */
    const T& getComponent(const EntityID id) const;

    /// @}

    /****************************************************
     * @name Component Assignment and Unassignment
     ****************************************************/
    /// @{

    /**
     * @brief Assign a component to an entity
     *
     * Component assignment is done by first checking it the entity ID has already been
     * listed in the sparseMap, if not, sparseMap is then resized, and all new elements is
     * set to NULL_INDEX.
     *
     * If the entity already has a component, than component assignment is just setting it
     * to the passed argument. Otherwise, if the entity not yet has a component, then,
     * the new component is appended to the densePool and the entity's ID also appended
     * to the denseMap.The sparse map is then updated to store the index of the component
     * and entity's ID in the densePool and denseMap.
     *
     * @param id Entity's ID for the accessed entity
     * @param component New component for the entity
     */
    void assignComponent(const EntityID id, const T& component);

    /**
     * @brief Unassign a component of an entity
     *
     * Component unassignment done by swap-and-pop operation to ensure that the array is
     * still dense. It is done by setting the densePool and denseMap at the index of the
     * removed entity to the last element of them. Then the last element is popped from the
     * back of the array. Last, the sparseMap of the entity to the removed component is
     * invalidated and the map of entity recently placed in the back of the pool to its
     * current position.
     *
     * @param id Entity's ID for the removed entity
     * @throw std::runtime_error if the entity has no component in this pool
     */
    void unassignComponent(const EntityID id) override;

    /// @}

private:
    std::vector<ComponentIndex> sparseMap;
    std::vector<EntityID> denseMap;
    std::vector<T> densePool;
};

} // namespace ecs

#include "componentPool.tpp"

#endif // SASAKALI_ECS_COMPONENT_POOL_H