#ifndef SASAKALI_ECS_TYPES_H
#define SASAKALI_ECS_TYPES_H

/**
 * @file types.h
 * @brief Header files that contains all types used by ECS Engine
 */

#include <cstddef>
#include <cstdint>
#include <limits>

namespace ecs {

/****************************************************
 * @name Types for entity functionalities
 ****************************************************/
/// @{

using EntityID = std::size_t;
using EntityGen = uint32_t;
using EntityMask = uint64_t;

static constexpr EntityMask NULL_MASK { 0 };

/**
 * @struct Entity types.h "include/types.h"
 * @brief A struct to group entity's ID and entity's generation number to be used as a handle
 */
struct Entity {
    EntityID id;
    EntityGen gen;

    bool operator==(const Entity& other) const {
        return id == other.id && gen == other.gen;
    }
};

/// @}

/****************************************************
 * @name Types for component pool functionalities
 ****************************************************/
/// @{

using ComponentIndex = std::size_t;

static constexpr ComponentIndex NULL_COMPONENT_INDEX { std::numeric_limits<ComponentIndex>::max() };

/// @}

/****************************************************
 * @name Types for component manager functionalities
 ****************************************************/
/// @{

using ComponentTypeID = std::size_t;

/// @}


} // namespace ecs

#endif // SASAKALI_ECS_TYPES_H