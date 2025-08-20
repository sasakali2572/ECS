add_executable(ecs_tests)
target_sources(ecs_tests PRIVATE main.cpp)
target_link_libraries(ecs_tests PRIVATE ecs)

add_test(NAME ECSCoreTests COMMAND ecs_tests)