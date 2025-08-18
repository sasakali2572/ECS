#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>

// Include your ECS headers
#include "types.h"
#include "entityManager.h"
#include "componentManager.h"

// A simple component type we can use for testing.
struct Position {
    float x;
    float y;
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
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
        ecs::EntityManager manager(10); // Max 10 entities for testing limit

        // Test Case: Initialization
        assert(manager.empty());
        assert(manager.size() == 0);
        std::cout << "  [PASS] Initial state." << std::endl;

        // Test Case: Entity Creation
        ecs::Entity entityA = manager.createEntity(1);
        ecs::Entity entityB = manager.createEntity(2);
        assert(manager.size() == 2);
        assert((entityA == ecs::Entity{0, 0}));
        assert((entityB == ecs::Entity{1, 0}));
        std::cout << "  [PASS] Entity creation." << std::endl;
        
        // Test Case: Entity Validation (isValid)
        assert(manager.isValid(entityA));
        ecs::Entity staleA = { entityA.id, 99 };
        assert(!manager.isValid(staleA));
        ecs::Entity invalidId = { 999, 0 };
        assert(!manager.isValid(invalidId));
        std::cout << "  [PASS] Entity validation." << std::endl;

        // Test Case: Entity Destruction
        manager.destroyEntity(entityA);
        assert(!manager.isValid(entityA));
        std::cout << "  [PASS] Entity destruction." << std::endl;

        // Test Case: Edge Case - ID Recycling
        ecs::Entity entityC = manager.createEntity(4);
        assert(entityC.id == entityA.id); // Recycled ID 0
        assert(entityC.gen == 1);         // Incremented generation
        assert(manager.isValid(entityC));
        std::cout << "  [PASS] ID recycling and generation." << std::endl;
        
        // Test Case: Edge Case - Max Entities Limit
        for(int i = 0; i < 7; ++i) { manager.createEntity(1); } // Fill up to 9 entities (0,1,2,3,4,5,6,7,8)
        ecs::Entity entityLast = manager.createEntity(1); // Entity 9, manager is now full
        assert(entityLast.id == 9);

        bool exceptionThrown = false;
        try {
            manager.createEntity(1); // Should fail
        } catch (const std::runtime_error& e) {
            exceptionThrown = true;
        }
        assert(exceptionThrown);
        std::cout << "  [PASS] Max entity limit." << std::endl;
    }
    std::cout << "[SUCCESS] EntityManager tests passed." << std::endl;


    // ========================================================================
    // Section 2: ComponentManager & ComponentPool Tests
    // ========================================================================
    std::cout << "\n[TESTING] ComponentManager..." << std::endl;
    {
        ecs::ComponentManager compManager;
        
        // Test Case: Registration
        compManager.registerComponentType<Position>();
        compManager.registerComponentType<Velocity>();
        assert(compManager.size() == 2);
        assert(compManager.isComponentTypeRegistered<Position>());
        assert(!compManager.isComponentTypeRegistered<int>()); // Test unregistered type
        std::cout << "  [PASS] Component type registration." << std::endl;

        // Test Case: Edge Case - Double Registration
        bool exceptionThrown = false;
        try {
            compManager.registerComponentType<Position>();
        } catch (const std::runtime_error& e) {
            exceptionThrown = true;
        }
        assert(exceptionThrown);
        std::cout << "  [PASS] Double registration prevention." << std::endl;
        
        // Test Cases: Component Assignment & Retrieval
        ecs::EntityID idA = 5, idB = 10, idC = 15;
        compManager.assignComponent<Position>(idA, { 1.f, 1.f });
        compManager.assignComponent<Position>(idB, { 2.f, 2.f });
        compManager.assignComponent<Position>(idC, { 3.f, 3.f });
        
        assert(compManager.hasComponent<Position>(idA));
        assert(!compManager.hasComponent<Velocity>(idA));
        compManager.getComponent<Position>(idB).x = 99.f;
        assert(compManager.getComponent<Position>(idB).x == 99.f);
        std::cout << "  [PASS] Component assignment and retrieval." << std::endl;

        // Test Case: Edge Case - Swap-and-Pop (Middle Element)
        compManager.unassignComponent<Position>(idB);
        assert(!compManager.hasComponent<Position>(idB));
        assert(compManager.hasComponent<Position>(idA));
        assert(compManager.hasComponent<Position>(idC));
        assert((compManager.getComponent<Position>(idC) == Position{3.f, 3.f})); // Check integrity of moved component
        std::cout << "  [PASS] Swap-and-pop (middle element)." << std::endl;
        
        // Test Case: Edge Case - Swap-and-Pop (Last Element)
        compManager.unassignComponent<Position>(idC);
        assert(!compManager.hasComponent<Position>(idC));
        assert(compManager.hasComponent<Position>(idA));
        std::cout << "  [PASS] Swap-and-pop (last element)." << std::endl;

        // Test Case: Edge Case - Swap-and-Pop (Only Element)
        compManager.unassignComponent<Position>(idA);
        assert(!compManager.hasComponent<Position>(idA));
        std::cout << "  [PASS] Swap-and-pop (only element)." << std::endl;

        // Test Case: System-Level entityDestroyed
        compManager.assignComponent<Position>(20, { 1.f, 1.f });
        compManager.assignComponent<Velocity>(20, { 2.f, 2.f });
        compManager.entityDestroyed(20);
        assert(!compManager.hasComponent<Position>(20));
        assert(!compManager.hasComponent<Velocity>(20));
        std::cout << "  [PASS] entityDestroyed broadcast." << std::endl;
    }
    std::cout << "[SUCCESS] ComponentManager tests passed." << std::endl;
    
    // ========================================================================
    std::cout << "\n--- All tests passed! Congratulations on building a complete ECS core! ---" << std::endl;

    return 0;
}