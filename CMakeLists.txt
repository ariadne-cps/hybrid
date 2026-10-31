cmake_minimum_required(VERSION 3.15)

project(Hybrid VERSION 1.0)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)

if(NOT TARGET ariadne-configuration)
    find_package(Git REQUIRED)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${CMAKE_CURRENT_SOURCE_DIR}"
                    submodule update --init -- ss/cfg
                    RESULT_VARIABLE _result)
    if(_result)
        message(FATAL_ERROR "Cannot initialize configuration submodule")
    endif()
    include("${CMAKE_CURRENT_SOURCE_DIR}/ss/cfg/cmake/ProjectOptions.cmake")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
    require_same_dependency_commit(Configuration CHAINS
        "configuration"
        "labelled|configuration"
    )
endif()

setup_project()
include_directories("${PROJECT_SOURCE_DIR}/include")
add_subdirectory(ss)

add_library(hybrid INTERFACE)
target_include_directories(hybrid INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/ariadne-hybrid>
)
target_link_libraries(hybrid INTERFACE labelled)

add_subdirectory(src)
target_link_libraries(HYBRID_SRC PRIVATE hybrid)

ariadne_register_public_headers(
    TARGET hybrid
    DIRECTORIES "${PROJECT_SOURCE_DIR}/include/hybrid"
    FILES "${PROJECT_SOURCE_DIR}/include/ariadne-hybrid.hpp"
)

setup_project_library(
    TARGET ariadne-hybrid
    OBJECTS HYBRID_SRC
    LINK_LIBRARIES hybrid
)

setup_standalone_project_tests(
    TARGET ariadne-hybrid
    TEST_DIRECTORY tests
    TEST_TARGET tests
    SOURCES
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/src"
)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    ariadne_install_dependency_bundle(
        TARGET hybrid
        DESTINATION include/ariadne-hybrid
    )
endif()

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
