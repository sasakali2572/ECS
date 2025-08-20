add_library(ecs entityManager.cpp componentManager.cpp scene.cpp)
target_include_directories(ecs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(ecs PUBLIC cxx_std_23)