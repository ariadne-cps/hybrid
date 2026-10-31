include(CTest)
set(UNIT_TESTS
    test_hybrid_set
    test_hybrid_scalings_header
    test_discrete_location
    test_hybrid_space
    test_hybrid_constraints
    test_hybrid_paving_operations
    test_hybrid_graphics_configuration
    test_hybrid_box_set
)
foreach(TEST IN LISTS UNIT_TESTS)
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} ariadne-hybrid hybrid)
    add_test(NAME ${TEST} COMMAND $<TARGET_FILE:${TEST}>)
    set_property(TEST ${TEST} PROPERTY LABELS "hybrid")
endforeach()
add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})
# test_hybrid_graphics.cpp is deliberately not registered: it writes graphics
# files but has no automatic behavioral assertions.
