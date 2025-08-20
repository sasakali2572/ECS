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
 *
 *
 * Systems in Scene:
 *
 * System as a part of ECS architecture is implemented in the Scene as an array that holds pointers
 * to available systems. In the array, systems are sorted by their priority value with lower valued
 * system is the one that will placed before the higher valued one. This system sorting is important
 * because when the Scene update, the systems will update consecutively according to its position in
 * the array.
 */

#include <limits>
#include <typeindex>
#include <memory>
#include <vector>
#include <unordered_map>
#include "types.h"
#include "entityManager.h"
#include "componentManager.h"
#include "system.h"

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
     * @name System Functionalities
     ****************************************************/
    /// @{

    /**
     * @brief Check whether a system is exists in the Scene
     * @return bool - true if the system is exists in the Scene, false otherwise
     */
    template<typename T>
    bool hasSystem() const;

    /**
     * @brief Returns a mutable reference to a system object
     * @return A mutable reference to a system object
     * @throw std::runtime_error if the system do not exists in the Scene
     */
    template<typename T>
    T& getSystem();

    /**
     * @brief Returns a read-only reference to a system object
     * @return A read-only reference to a system object
     * @throw std::runtime_error if the system do not exists in the Scene
     */
    template<typename T>
    const T& getSystem() const;

    /**
     * @brief add a system to the Scene
     * @throw std::runtime_error if the same system already exists in the Scene
     */
    template<typename T>
    void addSystem();

    /**
     * @brief remove a system from the Scene
     * @throw std::runtime_error if the system do not exists in the Scene
     */
    template<typename T>
    void removeSystem();

    /**
     * @brief Check whether a system is enabled
     * @return bool - true if the system is enabled, false otherwise
     * @throw std::runtime_error if the system do not exists in the Scene
     */
    template<typename T>
    bool isSystemEnabled() const;

    /**
     * @brief Set a system to be enabled/disabled
     * @param isEnabled A value to enable/disable a system. true for enabling, false for disabling
     * @throw std::runtime_error if the system do not exists in the Scene
     */
    template<typename T>
    void setSystemEnabled(bool isEnabled);

    /**
     * @brief Update the state of the scene by instructing each system to update
     * @param deltaTime elapsed time between last loop and current loop, essential for physics systems
     * @throw std::runtime_error if the system do not exists in the Scene
     */
    void update(float deltaTime);

    /// @}

    /****************************************************
     * @name High-Level Query
     ****************************************************/
    /// @{

    /**
     * @brief Returns an array of Entity handles that have a list of components
     * @return std::vector<Entity> - Array of Entity handles that have a list of components
     */
    template<typename... ComponentTypes>
    std::vector<Entity> getEntitiesWith() const;

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
    /**
     * @brief Sort systems in the Scene by their priority value
     * @throw std::runtime_error if system(s) doesn't provide the priority value
     */
    void sortSystems();

    /**
     * @brief Rebuild indexMap so it point to the correct system
     */
    void rebuildMap();

private:
    std::unordered_map<std::type_index, SystemIndex> indexMap;
    std::vector<std::unique_ptr<System>> systems;

    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;
};

} // namespace ecs

#include "scene.tpp"

#endif // SASAKALI_ECS_SCENE_H