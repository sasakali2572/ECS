include_directories(${CMAKE_HOME_DIRECTORY}/include)

add_executable(${PROJECT_NAME}_test)
target_sources(${PROJECT_NAME}_test PRIVATE test.cpp entityManager.cpp)