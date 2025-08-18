#include <iostream>
#include <cassert>
#include <stdexcept>

// Include your ECS headers
#include "types.h"
#include "scene.h"
#include "entityManager.h"
#include "componentManager.h"

// Define our component types for testing
struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx;
    float dy;
};

struct Tag {
    // A component with no data
};


int main() {
    std::cout << "--- Starting Scene Test Suite ---" << std::endl;

    // Test 1: Initialization and Component Registration
    std::cout << "\n[TESTING] Initialization and Registration..." << std::endl;
    ecs::Scene scene;
    scene.registerComponent<Position>();
    scene.registerComponent<Velocity>();
    scene.registerComponent<Tag>();

    assert(scene.getComponentManager().isComponentTypeRegistered<Position>());
    assert(scene.getComponentManager().isComponentTypeRegistered<Velocity>());
    std::cout << "  [PASS] Scene created managers and registered components." << std::endl;

    // Test 2: Entity Creation and Validation
    std::cout << "\n[TESTING] Entity Management..." << std::endl;
    ecs::Entity entityA = scene.createEntity();
    assert(scene.isEntityValid(entityA));
    assert(scene.getMask(entityA) == ecs::NULL_MASK); // Should have no components
    std::cout << "  [PASS] createEntity and isEntityValid work." << std::endl;

    // Test 3: Component Orchestration (Add)
    std::cout << "\n[TESTING] addComponent Orchestration..." << std::endl;
    scene.addComponent<Position>(entityA, { 10.f, 20.f });

    // Check 3.1: The component exists in the ComponentManager
    assert(scene.hasComponent<Position>(entityA));
    assert(!scene.hasComponent<Velocity>(entityA));

    // Check 3.2: The mask was correctly updated in the EntityManager
    ecs::EntityMask posMask = scene.getComponentManager().getComponentTypeMask<Position>();
    assert(posMask == 1);
    assert(scene.getMask(entityA) == posMask);
    std::cout << "  [PASS] addComponent correctly added data and updated the entity mask." << std::endl;

    // Add a second component
    scene.addComponent<Velocity>(entityA, { 1.f, 2.f });
    ecs::EntityMask velMask = scene.getComponentManager().getComponentTypeMask<Velocity>();
    assert(velMask == 2);
    assert(scene.getMask(entityA) == (posMask | velMask)); // Mask should be 3
    std::cout << "  [PASS] Adding a second component correctly updated the mask." << std::endl;

    // Test 4: Multi-component Checks
    bool hasComponentsResult;
    std::cout << "\n[TESTING] hasComponents..." << std::endl;
    hasComponentsResult = scene.hasComponents<Position, Velocity>(entityA);
    assert(hasComponentsResult);
    hasComponentsResult = scene.hasComponents<Position, Tag>(entityA);
    assert(!hasComponentsResult);
    std::cout << "  [PASS] hasComponents correctly checks for multiple components." << std::endl;

    // Test 5: Component Orchestration (Remove)
    std::cout << "\n[TESTING] removeComponent Orchestration..." << std::endl;
    scene.removeComponent<Position>(entityA);
    assert(!scene.hasComponent<Position>(entityA));
    assert(scene.hasComponent<Velocity>(entityA)); // Velocity should remain
    assert(scene.getMask(entityA) == velMask); // Mask should now only be the velocity mask
    std::cout << "  [PASS] removeComponent correctly removed data and updated the entity mask." << std::endl;

    // Test 6: Entity Destruction Orchestration
    std::cout << "\n[TESTING] destroyEntity Orchestration..." << std::endl;
    ecs::Entity entityB = scene.createEntity();
    scene.addComponent<Position>(entityB, { 0.f, 0.f });
    scene.addComponent<Tag>(entityB, {});

    ecs::EntityID b_id = entityB.id; // Save the ID for later checks

    assert(scene.isEntityValid(entityB));
    assert(scene.hasComponent<Position>(entityB));
    assert(scene.hasComponent<Tag>(entityB));

    scene.destroyEntity(entityB);

    // Check 6.1: The entity is no longer valid in the EntityManager
    assert(!scene.isEntityValid(entityB));

    // Check 6.2: The components were cleaned up in the ComponentManager
    // We use the raw ID here because the Entity handle is now invalid
    assert(!scene.getComponentManager().hasComponent<Position>(b_id));
    assert(!scene.getComponentManager().hasComponent<Tag>(b_id));
    std::cout << "  [PASS] destroyEntity correctly invalidated the entity and cleaned up components." << std::endl;


    // ========================================================================
    std::cout << "\n--- All Scene tests passed! Your ECS core is fully functional! ---" << std::endl;

    return 0;
}