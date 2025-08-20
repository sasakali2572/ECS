#ifndef SASAKALI_ECS_SYSTEM_H
#define SASAKALI_ECS_SYSTEM_H

/**
 * @file system.h
 * @brief Header file that contains System base class
 *
 * Overview:
 *
 * System base class is a system that provide a blueprint for creation of custom systems.
 * Several base functionalities of a system are: isEnabled and setEnabled to check and set
 * enabled value of the system, getPriority that returns system's priority value for systems
 * sorting, and update as the main functionalities of system that will modify the value
 * of components.
 */

#include "types.h"

namespace ecs {

class Scene;

/**
 * @class System system.h "include system.h"
 * @brief A base class that acts as a blueprints for costum systems
 */
class System {
public:
    virtual ~System() = default;

    /**
     * @brief Returns hard-coded system's priority value for sorting
     * @return SystemPriority - System's priority value for systems sorting
     */
    virtual SystemPriority getPriority() const = 0;

    /**
     * @brief Update components based on desired behaviour
     * @param scene A reference to a Scene object that will give access to Entities and Components
     * @param deltaTime elapsed time between last loop and current loop, essential for physics systems
     */
    virtual void update(Scene& scene, float deltaTime) = 0;

    /**
     * @brief Check whether the system is enabled
     * @return true if the system is enabled, false otherwise
     */
    bool isEnabled() const { return enabled; }

    /**
     * @brief Enable/disable the system
     * @param isEnabled value for enabled to set to, true for enable, false for disable
     */
    void setEnabled(bool isEnabled) { enabled = isEnabled; }

private:
    bool enabled = true;

};

} // namespace ecs

#endif // SASAKALI_ECS_SYSTEM_H