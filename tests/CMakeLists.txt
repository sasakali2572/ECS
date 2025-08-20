add_executable(ecs_tests main.cpp)
target_link_libraries(ecs_tests PRIVATE ecs)

add_test(NAME ECSCoreTests COMMAND ecs_tests)