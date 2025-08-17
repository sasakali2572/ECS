#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>

// Include your ECS headers
#include "types.h"
#include "entityManager.h"
#include "componentPool.h"

// A simple component type we can use for testing.
struct Position {
    float x;
    float y;

    // We add an equality operator to make assertions easier
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// A second component type for more advanced tests.
struct Velocity {
    float dx;
    float dy;
};


int main() {
    std::cout << "--- Starting ECS Test Suite ---" << std::endl;

    // ========================================================================
    // Section 1: EntityManager Tests
    // ========================================================================
    std::cout << "\n[TESTING] EntityManager..." << std::endl;
    {
        ecs::EntityManager manager;

        // Test 1.1: Initial State
        assert(manager.empty());
        assert(manager.size() == 0);
        std::cout << "  [PASS] Initial state is correct." << std::endl;

        // Test 1.2: Adding Entities
        ecs::Entity entityA = manager.createEntity(1); // Mask = 1
        ecs::Entity entityB = manager.createEntity(2); // Mask = 2
        ecs::Entity entityC = manager.createEntity(4); // Mask = 4

        assert(!manager.empty());
        assert(manager.size() == 3);
        assert((entityA == ecs::Entity{0, 0}));
        assert((entityB == ecs::Entity{1, 0}));
        assert((entityC == ecs::Entity{2, 0}));
        std::cout << "  [PASS] createEntity works and assigns sequential IDs." << std::endl;

        // Test 1.3: isValid
        assert(manager.isValid(entityA));
        assert(manager.isValid(entityB));
        ecs::Entity staleB = { entityB.id, 99 }; // Create a stale handle
        assert(!manager.isValid(staleB));
        ecs::Entity invalidId = { 999, 0 }; // Create an out-of-bounds handle
        assert(!manager.isValid(invalidId));
        std::cout << "  [PASS] isValid correctly validates handles." << std::endl;

        // Test 1.4: Removing an Entity and ID Recycling
        manager.destroyEntity(entityB);
        assert(!manager.isValid(entityB)); // Should no longer be active
        
        ecs::Entity entityD = manager.createEntity(16);
        assert(entityD.id == entityB.id); // Should reuse ID 1
        assert(entityD.gen == 1); // Should have the new generation
        assert(manager.isValid(entityD));
        std::cout << "  [PASS] destroyEntity and ID recycling work as expected." << std::endl;
    }
    std::cout << "[SUCCESS] EntityManager tests passed." << std::endl;


    // ========================================================================
    // Section 2: ComponentPool Tests
    // ========================================================================
    std::cout << "\n[TESTING] ComponentPool..." << std::endl;
    {
        ecs::ComponentPool<Position> posPool;
        ecs::EntityID idA = 5, idB = 10, idC = 15; // Use sparse IDs

        // Test 2.1: Initial State
        assert(posPool.empty());
        assert(posPool.size() == 0);
        std::cout << "  [PASS] Initial state is correct." << std::endl;

        // Test 2.2: Assigning and Checking Components
        posPool.assignComponent(idA, { 10.0f, 20.0f });
        posPool.assignComponent(idB, { 30.0f, 40.0f });

        assert(posPool.size() == 2);
        assert(posPool.hasComponent(idA));
        assert(posPool.hasComponent(idB));
        assert(!posPool.hasComponent(idC));
        assert(!posPool.hasComponent(999)); // Safe out-of-bounds check
        std::cout << "  [PASS] assignComponent and hasComponent work." << std::endl;

        // Test 2.3: Getting and Modifying Components
        Position posA_val = posPool.getComponent(idA);
        Position expectedPosA = { 10.0f, 20.0f };
        assert(posA_val == expectedPosA);
        
        posPool.getComponent(idB).x = 99.0f;
        assert(posPool.getComponent(idB).x == 99.0f);
        std::cout << "  [PASS] getComponent (read and write) works." << std::endl;

        // Test 2.4: Unassigning (Swap-and-Pop)
        posPool.assignComponent(idC, { 50.0f, 60.0f });
        assert(posPool.size() == 3);
        
        posPool.unassignComponent(idB); // Remove the middle element

        assert(posPool.size() == 2);
        assert(!posPool.hasComponent(idB));
        assert(posPool.hasComponent(idA));
        assert(posPool.hasComponent(idC));
        Position expectedPosC = { 50.0f, 60.0f };
        assert(posPool.getComponent(idC) == expectedPosC);
        std::cout << "  [PASS] unassignComponent (swap-and-pop) works correctly." << std::endl;
    }
    std::cout << "[SUCCESS] ComponentPool tests passed." << std::endl;

    // ========================================================================
    std::cout << "\n--- All tests passed! Congratulations! ---" << std::endl;

    return 0;
}