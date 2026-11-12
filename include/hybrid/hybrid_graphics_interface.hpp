/***************************************************************************
 *            hybrid/hybrid_graphics_interface.hpp
 *
 *  Copyright  2009-20  Davide Bresolin, Pieter Collins
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Hybrid.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file hybrid/hybrid_graphics_interface.hpp
 *  @brief Interfaces for hybrid graphics.
 */

#ifndef ARIADNE_HYBRID_GRAPHICS_INTERFACE_HPP
#define ARIADNE_HYBRID_GRAPHICS_INTERFACE_HPP

#include "labelled/graphics_interface.hpp"
#include "hybrid/hybrid_drawable_interface.hpp"

namespace Ariadne {

/** @brief Real represents interfaces for hybrid graphics. */
class Real;
template<class T> class Variable;
typedef Variable<Real> RealVariable;
template<class T> class Space;
typedef Space<Real> RealSpace;
/** @brief DiscreteLocation represents interfaces for hybrid graphics. */
class DiscreteLocation;

/** @brief Variables2d represents interfaces for hybrid graphics. */
struct Variables2d;

/** @brief Perform the hybrid-system operation. */
Bool valid_axis_variables(const RealSpace& space, const Variables2d& variables);

} // namespace Ariadne


#endif // ARIADNE_HYBRID_GRAPHICS_INTERFACE_HPP
