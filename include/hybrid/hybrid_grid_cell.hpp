/***************************************************************************
 *            hybrid/hybrid_grid_cell.hpp
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

/** @file hybrid/hybrid_grid_cell.hpp
 *  @brief Cells attached to discrete locations and named real coordinates.
 */

#ifndef ARIADNE_HYBRID_GRID_CELL_HPP
#define ARIADNE_HYBRID_GRID_CELL_HPP

#include "hybrid/hybrid_set.hpp"
#include "paving/grid_cell.hpp"

namespace Ariadne {

//! \ingroup HybridSetModule
//! \brief A cell associated with a grid in a hybrid space.
class HybridGridCell
    : public HybridBasicSet<GridCell> {
  public:
    /** @brief Construct or destroy an instance. */
    HybridGridCell() : HybridBasicSet<GridCell>() {}
    /** @brief Construct or destroy an instance. */
    HybridGridCell(DiscreteLocation q, const RealSpace& s, const GridCell& gc) : HybridBasicSet<GridCell>(q, s, gc) {}
    HybridExactBox box() const { return HybridExactBox(this->location(), this->space(), this->euclidean_set().box()); }
};

} // namespace Ariadne

#endif // ARIADNE_HYBRID_GRID_CELL_HPP
