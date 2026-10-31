include_directories(${Python3_INCLUDE_DIRS})
get_target_property(PYTHON_COMMON_SOURCE_DIR pyariadne-python-common SOURCE_DIR)

add_library(pyariadne-hybrid-submodule-obj OBJECT hybrid_submodule.cpp)
target_include_directories(pyariadne-hybrid-submodule-obj PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PYTHON_COMMON_SOURCE_DIR}/include"
    $<TARGET_PROPERTY:pyariadne-labelled,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(pyariadne-hybrid-submodule-obj PRIVATE
    pyariadne-python-common
    pybind11::module
    hybrid
)
set_target_properties(pyariadne-hybrid-submodule-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(pyariadne-hybrid INTERFACE)
target_sources(pyariadne-hybrid INTERFACE
    $<TARGET_OBJECTS:pyariadne-hybrid-submodule-obj>
)
target_include_directories(pyariadne-hybrid INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<TARGET_PROPERTY:pyariadne-labelled,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(pyariadne-hybrid INTERFACE pyariadne-labelled)

ariadne_add_python_module(NAME hybrid MODULE_SOURCE hybrid_module.cpp)
