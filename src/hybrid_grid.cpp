/***************************************************************************
 *            src/hybrid_grid.cpp
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

/** @file src/hybrid_grid.cpp
 *  @brief Materialize a concrete grid from a location's scaling policy.
 */

#include "algebra/vector.hpp"
#include "hybrid/hybrid_grid.hpp"
#include "paving/grid.hpp"

namespace Ariadne {

Grid HybridGrid::make_grid(const HybridScalings& scalings,
                           const DiscreteLocation& loc, const RealSpace& space) {
    Vector<ExactDouble> lengths(space.size());
    for (SizeType i = 0; i != space.size(); ++i) {
        lengths[i] = scalings.scaling(loc, space.variable(i));
    }
    return Grid(lengths);
}

Grid HybridGrid::operator[](const DiscreteLocation& loc) const {
    ARIADNE_PRECONDITION(this->_space.has_location(loc));
    return make_grid(this->_scalings, loc, this->_space[loc]);
}

Grid HybridGrid::grid(const DiscreteLocation& loc) const {
    return this->operator[](loc);
}

OutputStream& operator<<(OutputStream& os, const HybridGrid& hgrid) {
    return os << "HybridGrid( space=" << hgrid._space << ", scalings=" << hgrid._scalings << " )";
}

} // namespace Ariadne
