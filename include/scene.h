#ifndef SASAKALI_ECS_SCENE_H
#define SASAKALI_ECS_SCENE_H

/**
 * @file scene.h
 * @brief Header file that contains Scene class and its funcitionalities
 *
 * Overview:
 *
 * Scene class is an interface that let the EntityManager and ComponentManager to interact
 * between each others and provides an easy way to implement the system part of the ECS.
 * It is the "Orchestrator", "Manager of Managers", or "General Manager".
 *
 * Scene class is the one that owning the EntityManager and ComponentManager which implemented
 * as a unique_ptr to the manager objects. This class works by giving instructions to both
 * managers to do the task each of them specialized to do.
 */

#include <memory>
#include "types.h"
#include "entityManager.h"
#include "componentManager.h"

namespace ecs {

/**
 * @class Scene scene.h "include/scene.h"
 * @brief A high-level class that manage the interactions between EntityManager and
 * ComponentManager and provides a way to implement the system
 */
class Scene {
public:
    /****************************************************
     * @name Special Member Functions
     ****************************************************/
    /// @{

    /**
     * @brief Scene class' constructor, initialize entityManager and componentManager
     */
    Scene();

    /// @}

    /****************************************************
     * @name Entity Management API
     ****************************************************/
    /// @{

    /**
     * @brief Check whether an entity is a valid entity
     * @param entity A safe Entity handle for the checked entity
     * @return bool - true if the entity is valid, false otherwise
     */
    bool isEntityValid(const Entity& entity) const;

    /**
     * @brief Returns an entity's mask
     * @param entity A safe Entity handle for the accessed entity
     * @return EntityMask - Requested entity's mask
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    EntityMask getMask(const Entity& entity) const;

    /**
     * @brief Create a new entity and set it mask to NULL_MASK
     * @return Entity - A safe Entity handle for the new entity
     */
    Entity createEntity();

    /**
     * @brief Destroy an entity and remove all its components from their corresponding pools
     * @param entity A safe Entity handle for the destroyed entity
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    void destroyEntity(const Entity& entity);

    /// @}

    /****************************************************
     * @name Component Management API
     ****************************************************/
    /// @{

    /**
     * @brief Register a new component type to the component manager
     */
    template<typename T>
    void registerComponent();

    /**
     * @brief Check whether an entity has a component of a specific type
     * @param entity A safe Entity handle for the checked entity
     * @return bool - true if the entity has component of requested type
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    template<typename T>
    bool hasComponent(const Entity& entity) const;

    /**
     * @brief Check whether an entity has components of all the specified type
     * @param entity A safe Entity handle for the checked entity
     * @return bool - true if the entity has components of all requested type
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    template<typename ...ComponentTypes>
    bool hasComponents(const Entity& entity) const;

    /**
     * @brief Return a mutable reference to an entity's component of a specific component type
     * @param entity A safe Entity handle for the checked entity
     * @return T& - A mutable reference to the entity's component
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    template<typename T>
    T& getComponent(const Entity& entity);

    /**
     * @brief Return a read-only reference to an entity's component of a specific component type
     * @param entity A safe Entity handle for the checked entity
     * @return const T& - A read-only reference to the entity's component
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    template <typename T>
    const T& getComponent(const Entity& entity) const;

    /**
     * @brief Add a component of a specific type to an entity and update its mask
     * @param entity A safe Entity handle for the accessed entity
     * @param component Component to be added and associated with the entity
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    template<typename T>
    void addComponent(const Entity& entity, T component);

    /**
     * @brief Remove a component of a specific type from an entity and update its mask
     * @param entity A safe Entity handle for the accessed entity
     * @throw std::runtime_error if requested entity is not a valid entity
     */
    template<typename T>
    void removeComponent(const Entity& entity);

    /// @}

    /****************************************************
     * @name Managers Direct Access
     ****************************************************/
    /// @{

    /**
     * @brief Returns a read-only reference to the entity manager for direct access
     * @return A read-only reference to the entity manager
     */
    const EntityManager& getEntityManager() const;

    /**
     * @brief Returns a read-only reference to the component manager for direct access
     * @return A read-only reference to the component manager
     */
    const ComponentManager& getComponentManager() const;

    /// @}


private:
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;

};

} // namespace ecs

#include "scene.tpp"

#endif // SASAKALI_ECS_SCENE_H