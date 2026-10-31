find_library(ARIADNE_HYBRID_LIBRARY NAMES ariadne-hybrid)
find_path(ARIADNE_HYBRID_INCLUDE_DIR ariadne-hybrid.hpp
          PATH_SUFFIXES ariadne-hybrid)

find_package(Threads REQUIRED)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AriadneHybrid DEFAULT_MSG
    ARIADNE_HYBRID_LIBRARY
    ARIADNE_HYBRID_INCLUDE_DIR
)

if(AriadneHybrid_FOUND)
    get_filename_component(ARIADNE_HYBRID_INCLUDE_PARENT_DIR
        "${ARIADNE_HYBRID_INCLUDE_DIR}" DIRECTORY)

    if(NOT TARGET AriadneHybrid::ariadne-hybrid)
        add_library(AriadneHybrid::ariadne-hybrid UNKNOWN IMPORTED)
        set_target_properties(AriadneHybrid::ariadne-hybrid PROPERTIES
            IMPORTED_LOCATION "${ARIADNE_HYBRID_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES
                "${ARIADNE_HYBRID_INCLUDE_PARENT_DIR};${ARIADNE_HYBRID_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "Threads::Threads"
        )
    endif()
endif()
