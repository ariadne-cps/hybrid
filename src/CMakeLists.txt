add_library(HYBRID_SRC OBJECT
    discrete_location.cpp
    hybrid_constraint_set.cpp
    hybrid_bounded_constraint_set.cpp
    hybrid_set.cpp
    hybrid_set_adapters.cpp
    hybrid_paving.cpp
    hybrid_graphics.cpp
)
target_include_directories(HYBRID_SRC PRIVATE "${PROJECT_SOURCE_DIR}/include/hybrid")
