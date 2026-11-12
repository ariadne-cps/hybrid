/***************************************************************************
 *            python/bindings/hybrid_submodule.cpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne Hybrid.
 *
 *  Ariadne Hybrid is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne Hybrid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Hybrid. If not, see <https://www.gnu.org/licenses/>.
 */

/** @file python/bindings/hybrid_submodule.cpp
 *  @brief Bind hybrid objects and functions to Python.
 */

#include "hybrid/discrete_location.hpp"
#include "hybrid/hybrid_graphics_interface.hpp"
#include "hybrid/hybrid_grid.hpp"
#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_set.hpp"
#include "hybrid/hybrid_space.hpp"
#include "pybind11.hpp"

#include "function-utilities.hpp"

using namespace Ariadne;

void hybrid_submodule(pybind11::module &module) {
    pybind11::class_<DiscreteLocation> location_class(module, "DiscreteLocation");
    location_class.def(pybind11::init<>());
    location_class.def(pybind11::init<Int>());
    location_class.def(pybind11::init<StringVariable, String>());
    location_class.def("adjoin", &DiscreteLocation::adjoin);
    location_class.def("__eq__", [](const DiscreteLocation &a, const DiscreteLocation &b) { return a == b; });
    location_class.def("__ne__", [](const DiscreteLocation &a, const DiscreteLocation &b) { return a != b; });
    location_class.def("__str__", &__cstr__<DiscreteLocation>);
    location_class.def("__repr__", &__cstr__<DiscreteLocation>);

    module.def("join", (DiscreteLocation (*)(const DiscreteLocation &, const DiscreteLocation &))&join);
    module.def("are_same", &are_same);
    module.def("are_distinguishable", &are_distinguishable);
    module.def("is_restriction", &is_restriction);

    pybind11::class_<HybridSpaceInterface> space_interface_class(module, "HybridSpaceInterface");
    pybind11::class_<MonolithicHybridSpace, pybind11::bases<HybridSpaceInterface>>
        monolithic_class(module, "MonolithicHybridSpace");
    monolithic_class.def(pybind11::init<>());
    monolithic_class.def("new_location", &MonolithicHybridSpace::new_location);
    monolithic_class.def("has_location", &MonolithicHybridSpace::has_location);
    monolithic_class.def("space", [](const MonolithicHybridSpace &s, const DiscreteLocation &q) { return s[q]; });
    monolithic_class.def("__str__", &__cstr__<MonolithicHybridSpace>);
    monolithic_class.def("__repr__", &__cstr__<MonolithicHybridSpace>);

    pybind11::class_<HybridSpace> space_class(module, "HybridSpace");
    space_class.def(pybind11::init<const HybridSpaceInterface &>());
    space_class.def("has_location", &HybridSpace::has_location);
    space_class.def("space", [](const HybridSpace &s, const DiscreteLocation &q) { return s[q]; });
    space_class.def("__str__", &__cstr__<HybridSpace>);
    space_class.def("__repr__", &__cstr__<HybridSpace>);

    pybind11::class_<HybridGrid> grid_class(module, "HybridGrid");
    grid_class.def(pybind11::init<const HybridSpace &>());
    grid_class.def("has_location", &HybridGrid::has_location);
    grid_class.def("grid", &HybridGrid::grid);
    grid_class.def("__str__", &__cstr__<HybridGrid>);
    grid_class.def("__repr__", &__cstr__<HybridGrid>);

    pybind11::class_<HybridDrawableInterface> drawable_class(module, "HybridDrawableInterface");

    pybind11::class_<HybridRealBox, pybind11::bases<HybridDrawableInterface>>
        box_class(module, "HybridRealBox");
    box_class.def(pybind11::init<const DiscreteLocation &, const RealSpace &, const RealBox &>());
    box_class.def("location", &HybridRealBox::location);
    box_class.def("space", &HybridRealBox::space);
    box_class.def("dimension", &HybridRealBox::dimension);
    box_class.def("euclidean_set", [](const HybridRealBox &b) { return b.euclidean_set(); });
    box_class.def("__str__", &__cstr__<HybridRealBox>);
    box_class.def("__repr__", &__cstr__<HybridRealBox>);

    pybind11::class_<HybridPaving, pybind11::bases<HybridDrawableInterface>>
        paving_class(module, "HybridPaving");
    paving_class.def(pybind11::init<const HybridGrid &>());
    paving_class.def("has_location", &HybridPaving::has_location);
    paving_class.def("is_empty", &HybridPaving::is_empty);
    paving_class.def("size", &HybridPaving::size);
    paving_class.def("clear", &HybridPaving::clear);
    paving_class.def("__str__", &__cstr__<HybridPaving>);
    paving_class.def("__repr__", &__cstr__<HybridPaving>);
}
