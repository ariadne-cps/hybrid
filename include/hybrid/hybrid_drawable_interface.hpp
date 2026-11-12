/***************************************************************************
 *            hybrid/hybrid_drawable_interface.hpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

/** @file hybrid/hybrid_drawable_interface.hpp
 *  @brief Interface for drawing hybrid-space objects.
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

// Abstract drawing hook used by hybrid objects. No rendering dependency.
#ifndef ARIADNE_HYBRID_DRAWABLE_INTERFACE_HPP
#define ARIADNE_HYBRID_DRAWABLE_INTERFACE_HPP

#include "hybrid/discrete_location.hpp"
#include "utility/container.hpp"

namespace Ariadne {

/** @brief CanvasInterface represents interface for drawing hybrid-space objects. */
class CanvasInterface;
/** @brief Variables2d represents interface for drawing hybrid-space objects. */
struct Variables2d;

//! \ingroup GraphicsModule
//! \brief Base interface for drawable objects
class HybridDrawableInterface {
  public:
    //! brief The type of data needed to project to a 2d image.
    typedef Variables2d ProjectionType;
    //! brief Virtual destructor.
    virtual ~HybridDrawableInterface() = default;
    //! brief Draw the object on the canvas \a c using line segments and fill/stroke commands.
    virtual Void draw(CanvasInterface& c, const Set<DiscreteLocation>& q, const Variables2d& v) const = 0;
};

} // namespace Ariadne

#endif // ARIADNE_HYBRID_DRAWABLE_INTERFACE_HPP
