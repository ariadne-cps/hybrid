/***************************************************************************
 *            tests/test_hybrid_grid_cell_header.cpp
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

/** @file tests/test_hybrid_grid_cell_header.cpp
 *  @brief Check concrete hybrid cells can be imported independently.
 */

#include "hybrid/hybrid_grid_cell.hpp"
#include <type_traits>

static_assert(std::is_base_of_v<Ariadne::HybridBasicSet<Ariadne::GridCell>, Ariadne::HybridGridCell>);
#ifdef ARIADNE_PAVING_BINARY_TREE_PAVING_HPP
#error HybridGridCell must not force a binary-tree backend
#endif
#ifdef ARIADNE_PAVING_ROBDD_PAVING_HPP
#error HybridGridCell must not force a ROBDD backend
#endif

int main() { return 0; }
