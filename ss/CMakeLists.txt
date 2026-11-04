if(NOT TARGET labelled)
    ariadne_add_submodule(labelled)
endif()

# The Testing dependency is test-only; reuse an existing transitive target.
if(NOT TARGET ariadne-testing)
    ariadne_add_submodule(testing)
endif()
