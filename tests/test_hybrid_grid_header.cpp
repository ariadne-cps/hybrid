/***************************************************************************
 *            tests/test_hybrid_grid_header.cpp
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

/** @file tests/test_hybrid_grid_header.cpp
 *  @brief Check that HybridGrid's public header does not import concrete paving grids.
 */

#include "hybrid/hybrid_grid.hpp"

#ifdef ARIADNE_GRID_HPP
#error HybridGrid must not import paving/grid.hpp
#endif
#ifdef ARIADNE_GRID_CELL_HPP
#error HybridGrid must not import paving/grid_cell.hpp
#endif

#include <type_traits>

static_assert(std::is_copy_constructible_v<Ariadne::HybridGrid>);
static_assert(std::is_copy_assignable_v<Ariadne::HybridGrid>);


int main() { return 0; }
