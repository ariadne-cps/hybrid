/***************************************************************************
 *            tests/test_hybrid_header_boundaries.cpp
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

/** @file tests/test_hybrid_header_boundaries.cpp
 *  @brief Prevent symbolic Hybrid interfaces from importing concrete backends.
 */

// The polymorphic Hybrid set interface only needs geometry-level set APIs.
#include "hybrid/hybrid_set_interface.hpp"

#ifdef ARIADNE_EXPRESSION_SET_HPP
#error Hybrid set interface must not import expression-set/expression_set.hpp
#endif
#ifdef ARIADNE_GRID_HPP
#error Hybrid set interface must not import concrete paving/grid.hpp
#endif

// Expression-defined Hybrid sets may use expression-set, but must not force
// concrete gridding, paving or tree backends into symbolic-only consumers.
#include "hybrid/hybrid_expression_set.hpp"

#ifdef ARIADNE_GRID_HPP
#error Hybrid expression sets must not import concrete paving/grid.hpp
#endif
#ifdef ARIADNE_PAVING_BINARY_TREE_PAVING_HPP
#error Hybrid expression sets must not import binary_tree_paving.hpp
#endif

#include <type_traits>

static_assert(std::is_abstract_v<Ariadne::HybridSetInterfaceBase>);
static_assert(std::is_abstract_v<Ariadne::EffectiveHybridSetInterface>);
static_assert(std::is_abstract_v<Ariadne::ValidatedHybridSetInterface>);
static_assert(std::is_base_of_v<Ariadne::EffectiveHybridSetInterface, Ariadne::HybridBoxSet>);

// The generic Hybrid set still exposes GridCell, but not tree/ROBDD backends.
#include "hybrid/hybrid_set.hpp"

#ifdef ARIADNE_PAVING_BINARY_TREE_PAVING_HPP
#error Hybrid set header must not import the binary tree paving backend
#endif
#ifdef ARIADNE_PAVING_ROBDD_PAVING_HPP
#error Hybrid set header must not import the ROBDD paving backend
#endif

// HybridPaving selects concrete backends in its implementation, not its API.
#include "hybrid/hybrid_paving.hpp"

#ifdef ARIADNE_PAVING_BINARY_TREE_PAVING_HPP
#error HybridPaving header must not import the binary tree paving backend
#endif
#ifdef ARIADNE_PAVING_ROBDD_PAVING_HPP
#error HybridPaving header must not import the ROBDD paving backend
#endif

int main() { return 0; }
