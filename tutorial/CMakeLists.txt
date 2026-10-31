cmake_minimum_required(VERSION 3.15)
project(AriadneHybridTutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile against the installed library, as in symbolic/tutorials.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(AriadneHybrid REQUIRED)

add_executable(tutorial_hybrid tutorial_hybrid.cpp)
target_link_libraries(tutorial_hybrid PRIVATE AriadneHybrid::ariadne-hybrid)
