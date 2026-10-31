#include "pybind11.hpp"

void hybrid_submodule(pybind11::module& module);

void hybrid_module(pybind11::module& module) {
    hybrid_submodule(module);
}
