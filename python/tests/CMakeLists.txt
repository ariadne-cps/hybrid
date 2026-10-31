execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pytest --version
    RESULT_VARIABLE PYTEST_result
    OUTPUT_QUIET ERROR_QUIET
)

if(PYTEST_result EQUAL 0)
    set(PYTHON_TESTS ON)
elseif(PYTHON_ENFORCE_TESTS)
    message(FATAL_ERROR "pytest is not available for ${Python3_EXECUTABLE}.")
else()
    message(WARNING "pytest is not available; Python tests will not be performed.")
endif()

if(PYTHON_TESTS)
    add_custom_target(test_python_hybrid
        COMMAND "${Python3_EXECUTABLE}" -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/test_hybrid.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    add_test(
        NAME test_python_hybrid
        COMMAND "${Python3_EXECUTABLE}" -m pytest "${CMAKE_CURRENT_SOURCE_DIR}/test_hybrid.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    set_property(TEST test_python_hybrid PROPERTY
        ENVIRONMENT "PYTHONPATH=$<TARGET_FILE_DIR:pyariadne>"
    )
    set_property(TEST test_python_hybrid PROPERTY LABELS "python;hybrid")
    add_dependencies(test_python_hybrid pyariadne)
    add_dependencies(tests pyariadne)
endif()
