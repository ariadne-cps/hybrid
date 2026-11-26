/***************************************************************************
 *            tests/test_hybrid_paving_header.cpp
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

/** @file tests/test_hybrid_paving_header.cpp
 *  @brief Ensure the HybridPaving API does not import concrete paving backends.
 */

#include "hybrid/hybrid_paving.hpp"

#ifdef ARIADNE_PAVING_BINARY_TREE_PAVING_HPP
#error HybridPaving must not import binary_tree_paving.hpp
#endif
#ifdef ARIADNE_PAVING_ROBDD_PAVING_HPP
#error HybridPaving must not import robdd_paving.hpp
#endif

#include <type_traits>

static_assert(std::is_copy_constructible_v<Ariadne::HybridPaving>);
static_assert(std::is_base_of_v<Ariadne::HybridDrawableInterface, Ariadne::HybridPaving>);


int main() { return 0; }
