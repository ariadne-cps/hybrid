#include "pybind11.hpp"
#include "function-utilities.hpp"

#include "hybrid/discrete_location.hpp"
#include "hybrid/hybrid_space.hpp"
#include "hybrid/hybrid_grid.hpp"
#include "hybrid/hybrid_set.hpp"
#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_graphics_interface.hpp"

using namespace Ariadne;

void hybrid_submodule(pybind11::module& module) {
    pybind11::class_<DiscreteLocation> location_class(module, "DiscreteLocation");
    location_class.def(pybind11::init<>());
    location_class.def(pybind11::init<Int>());
    location_class.def(pybind11::init<StringVariable, String>());
    location_class.def("adjoin", &DiscreteLocation::adjoin);
    location_class.def("__eq__", [](DiscreteLocation const& a, DiscreteLocation const& b) { return a == b; });
    location_class.def("__ne__", [](DiscreteLocation const& a, DiscreteLocation const& b) { return a != b; });
    location_class.def("__str__", &__cstr__<DiscreteLocation>);
    location_class.def("__repr__", &__cstr__<DiscreteLocation>);

    module.def("join", (DiscreteLocation(*)(DiscreteLocation const&, DiscreteLocation const&)) &join);
    module.def("are_same", &are_same);
    module.def("are_distinguishable", &are_distinguishable);
    module.def("is_restriction", &is_restriction);

    pybind11::class_<HybridSpaceInterface> space_interface_class(module, "HybridSpaceInterface");
    pybind11::class_<MonolithicHybridSpace, pybind11::bases<HybridSpaceInterface>>
        monolithic_class(module, "MonolithicHybridSpace");
    monolithic_class.def(pybind11::init<>());
    monolithic_class.def("new_location", &MonolithicHybridSpace::new_location);
    monolithic_class.def("has_location", &MonolithicHybridSpace::has_location);
    monolithic_class.def("space", [](MonolithicHybridSpace const& s, DiscreteLocation const& q) { return s[q]; });
    monolithic_class.def("__str__", &__cstr__<MonolithicHybridSpace>);
    monolithic_class.def("__repr__", &__cstr__<MonolithicHybridSpace>);

    pybind11::class_<HybridSpace> space_class(module, "HybridSpace");
    space_class.def(pybind11::init<HybridSpaceInterface const&>());
    space_class.def("has_location", &HybridSpace::has_location);
    space_class.def("space", [](HybridSpace const& s, DiscreteLocation const& q) { return s[q]; });
    space_class.def("__str__", &__cstr__<HybridSpace>);
    space_class.def("__repr__", &__cstr__<HybridSpace>);

    pybind11::class_<HybridGrid> grid_class(module, "HybridGrid");
    grid_class.def(pybind11::init<HybridSpace const&>());
    grid_class.def("has_location", &HybridGrid::has_location);
    grid_class.def("grid", &HybridGrid::grid);
    grid_class.def("__str__", &__cstr__<HybridGrid>);
    grid_class.def("__repr__", &__cstr__<HybridGrid>);

    pybind11::class_<HybridDrawableInterface> drawable_class(module, "HybridDrawableInterface");

    pybind11::class_<HybridRealBox, pybind11::bases<HybridDrawableInterface>>
        box_class(module, "HybridRealBox");
    box_class.def(pybind11::init<DiscreteLocation const&, RealSpace const&, RealBox const&>());
    box_class.def("location", &HybridRealBox::location);
    box_class.def("space", &HybridRealBox::space);
    box_class.def("dimension", &HybridRealBox::dimension);
    box_class.def("euclidean_set", [](HybridRealBox const& b) { return b.euclidean_set(); });
    box_class.def("__str__", &__cstr__<HybridRealBox>);
    box_class.def("__repr__", &__cstr__<HybridRealBox>);

    pybind11::class_<HybridBinaryTreePaving, pybind11::bases<HybridDrawableInterface>>
        paving_class(module, "HybridBinaryTreePaving");
    paving_class.def(pybind11::init<HybridGrid const&>());
    paving_class.def("has_location", &HybridBinaryTreePaving::has_location);
    paving_class.def("is_empty", &HybridBinaryTreePaving::is_empty);
    paving_class.def("size", &HybridBinaryTreePaving::size);
    paving_class.def("clear", &HybridBinaryTreePaving::clear);
    paving_class.def("__str__", &__cstr__<HybridBinaryTreePaving>);
    paving_class.def("__repr__", &__cstr__<HybridBinaryTreePaving>);
}
