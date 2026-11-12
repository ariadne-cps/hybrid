/***************************************************************************
 *            hybrid/hybrid_augmented_paving_draw.hpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

/** @file hybrid/hybrid_augmented_paving_draw.hpp
 *  @brief Drawing adapters for hybrid augmented pavings.
 */

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

// Temporary adapter; final home will be the drawing repository.
#ifndef ARIADNE_REASONING_HYBRID_AUGMENTED_PAVING_DRAW_HPP
#define ARIADNE_REASONING_HYBRID_AUGMENTED_PAVING_DRAW_HPP

#include "hybrid/hybrid_augmented_paving.hpp"
#include "hybrid/hybrid_drawable_interface.hpp"

namespace Ariadne {

inline Void draw(CanvasInterface& canvas, Set<DiscreteLocation> const& locations,
                 Variables2d const& axes, HybridAugmentedPaving const& paving) {
    /** @brief Perform the hybrid-system operation. */
    paving.state_auxiliary_paving().draw(canvas,locations,axes);
}

/** @brief HybridAugmentedPavingDrawable represents drawing adapters for hybrid augmented pavings. */
class HybridAugmentedPavingDrawable final : public HybridDrawableInterface {
  public:
    /** @brief Construct or destroy an instance. */
    explicit HybridAugmentedPavingDrawable(HybridAugmentedPaving const& paving) : _paving(paving) { }
    Void draw(CanvasInterface& canvas, Set<DiscreteLocation> const& locations,
              Variables2d const& axes) const override {
        /** @brief Render the object. */
        Ariadne::draw(canvas,locations,axes,_paving);
    }
  private:
    HybridAugmentedPaving _paving;
};

} // namespace Ariadne
#endif // ARIADNE_REASONING_HYBRID_AUGMENTED_PAVING_DRAW_HPP
