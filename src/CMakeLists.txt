# add_executable(${PROJECT_NAME}_test)
# target_sources(${PROJECT_NAME}_test PRIVATE test.cpp entityManager.cpp)
# target_include_directories(${PROJECT_NAME}_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(ecs entityManager.cpp)
target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(ecs PUBLIC cxx_std_23)