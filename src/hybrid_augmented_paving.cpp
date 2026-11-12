/***************************************************************************
 *            src/hybrid_augmented_paving.cpp
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

/** @file src/hybrid_augmented_paving.cpp
 *  @brief Implement augmented hybrid pavings.
 */

#include "hybrid/hybrid_augmented_paving.hpp"

#include "function/functional.hpp"

namespace Ariadne {
HybridPaving HybridAugmentedPaving::state_auxiliary_paving() const {
    HybridGrid state_grid = this->state_paving().grid();
    HybridScalings scalings = state_grid.scalings();
    HybridGrid result_grid(this->_auxiliary_mapping.state_auxiliary_space(), scalings);
    HybridPaving result = this->state_paving().with_grid(result_grid);
    for (auto it = this->state_paving().locations_begin(); it != this->state_paving().locations_end(); ++it) {
        const DiscreteLocation &q = it->first;
        const Paving &paving = it->second;
        LabelledMapping mapping = this->_auxiliary_mapping(q);
        ARIADNE_PRECONDITION(state_grid.space(q) == mapping.argument_space());
        Grid auxiliary_grid = scalings.grid(q, mapping.result_space());
        // Auxiliary product is a derived tree view, not authoritative storage.
        BinaryTreePaving tree(paving.grid());
        for (auto cell = paving.begin(), end = paving.end(); cell != end; ++cell)
            tree.adjoin(*cell);
        auto product = outer_skew_product(tree, auxiliary_grid, mapping.mapping());
        // Copy geometry through the selected backend, not a Tree handle.
        result[q].adjoin(static_cast<const SubPavingInterface &>(product));
    }
    return result;
}
} // namespace Ariadne
