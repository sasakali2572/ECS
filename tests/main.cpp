#include <iostream>
#include <cassert>
#include <stdexcept>

// Include your ECS headers
#include "types.h"
#include "scene.h"

// --- Define Test Components ---
struct Position { float x, y; };
struct Velocity { float dx, dy; };
struct Tag { /* empty */ };

// --- Define Test Systems ---
class MovementSystem : public ecs::System {
public:
    ecs::SystemPriority getPriority() const override { return 200; }
    void update(ecs::Scene& scene, float deltaTime) override {
        for (ecs::Entity entity : scene.getEntitiesWith<Position, Velocity>()) {
            auto& pos = scene.getComponent<Position>(entity);
            const auto& vel = scene.getComponent<Velocity>(entity);
            pos.x += vel.dx * deltaTime;
            pos.y += vel.dy * deltaTime;
        }
    }
};

class TerminationSystem : public ecs::System {
public:
    ecs::SystemPriority getPriority() const override { return 100; } // Runs before Movement
    void update(ecs::Scene& scene, float deltaTime) override {
        // This system destroys any entity that has a Position and a Tag component
        for (ecs::Entity entity : scene.getEntitiesWith<Position, Tag>()) {
            scene.destroyEntity(entity);
        }
    }
};


int main() {
    std::cout << "--- Starting Full ECS Engine Test Suite ---" << std::endl;
    
    ecs::Scene scene;

    // Test 1: Registration
    std::cout << "\n[TESTING] Registration..." << std::endl;
    scene.registerComponent<Position>();
    scene.registerComponent<Velocity>();
    scene.registerComponent<Tag>();
    scene.addSystem<MovementSystem>();
    scene.addSystem<TerminationSystem>();
    assert(scene.hasSystem<MovementSystem>());
    assert(scene.hasSystem<TerminationSystem>());
    std::cout << "  [PASS] All components and systems registered." << std::endl;

    // Test 2: Entity & Component Setup
    std::cout << "\n[TESTING] Entity and Component Setup..." << std::endl;
    ecs::Entity movingEntity = scene.createEntity();
    scene.addComponent<Position>(movingEntity, { 10.f, 10.f });
    scene.addComponent<Velocity>(movingEntity, { 5.f, 2.f });

    ecs::Entity staticEntity = scene.createEntity();
    scene.addComponent<Position>(staticEntity, { 100.f, 100.f });

    ecs::Entity doomedEntity = scene.createEntity();
    scene.addComponent<Position>(doomedEntity, { 50.f, 50.f });
    scene.addComponent<Tag>(doomedEntity, {});
    
    std::vector<ecs::Entity> filteredEntities;
    assert(scene.getEntitiesWith<Position>().size() == 3);
    filteredEntities = scene.getEntitiesWith<Position, Velocity>();
    assert(filteredEntities.size() == 1);
    filteredEntities = scene.getEntitiesWith<Position, Tag>();
    assert(filteredEntities.size() == 1);
    std::cout << "  [PASS] Entities created and components assigned." << std::endl;

    // Test 3: Scene Update Loop
    std::cout << "\n[TESTING] Scene::update()..." << std::endl;
    // Simulate one frame of time (1.0 seconds for easy math)
    scene.update(1.0f);

    // Check 3.1: TerminationSystem should have run first and destroyed doomedEntity
    assert(!scene.isEntityValid(doomedEntity));
    assert(scene.getEntitiesWith<Position>().size() == 2); // Should be 2 now
    std::cout << "  [PASS] TerminationSystem ran correctly (priority 100)." << std::endl;

    // Check 3.2: MovementSystem should have run second and updated movingEntity
    assert(scene.isEntityValid(movingEntity));
    assert(scene.getComponent<Position>(movingEntity).x == 15.f); // 10 + 5*1.0
    assert(scene.getComponent<Position>(movingEntity).y == 12.f); // 10 + 2*1.0
    
    // Check 3.3: staticEntity should not have moved
    assert(scene.getComponent<Position>(staticEntity).x == 100.f);
    std::cout << "  [PASS] MovementSystem ran correctly (priority 200)." << std::endl;

    // Test 4: System Management
    std::cout << "\n[TESTING] System Management..." << std::endl;
    scene.getSystem<MovementSystem>().setEnabled(false);
    assert(!scene.isSystemEnabled<MovementSystem>());
    
    scene.update(1.0f); // Simulate another frame
    // Position should NOT have changed this time
    assert(scene.getComponent<Position>(movingEntity).x == 15.f);
    std::cout << "  [PASS] System enable/disable works." << std::endl;

    scene.removeSystem<MovementSystem>();
    assert(!scene.hasSystem<MovementSystem>());
    std::cout << "  [PASS] System removal works." << std::endl;

    std::cout << "\n--- All tests passed! Your ECS Engine is complete and robust! ---" << std::endl;

    return 0;
}