/***************************************************************************
 *            hybrid/hybrid_graphics.cpp
 *
 *  Copyright  2011-20  Pieter Collins
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

/** @file src/hybrid_graphics.cpp
 *  @brief Implement graphical representations of hybrid sets.
 */

#include "hybrid/hybrid_graphics.hpp"

#include "function/functional.hpp"
#include "geometry/box.hpp"
#include "geometry/point.hpp"
#include "hybrid/discrete_location.hpp"
#include "numeric/numeric.hpp"
#include "rendering/figure.hpp"
#include "rendering/geometry2d.hpp"
#include "rendering/rendering_manager.hpp"
#include "set/function_set.hpp"
#include "symbolic/space.hpp"
#include "utility/macros.hpp"
#include "utility/stlio.hpp"

#include "expression-set/expression_set.hpp"

namespace Ariadne {

static const Nat HYBRID_DEFAULT_WIDTH = 800;
static const Nat HYBRID_DEFAULT_HEIGHT = 800;

HybridFigure::~HybridFigure() {
}

HybridFigure::HybridFigure()
    : variables(RealVariable("x"), RealVariable("y")) {
}

Void HybridFigure::write(const char* cfilename) const {
    this->write(cfilename, HYBRID_DEFAULT_WIDTH, HYBRID_DEFAULT_HEIGHT);
}

Void HybridFigure::write(const char* cfilename, Nat drawing_width, Nat drawing_height) const {
    // RenderingManager owns the configured backend, including the portable
    // raster backend. No Cairo-specific capability is needed at this layer.
    SharedPointer<CanvasInterface> canvas = RenderingManager::instance().backend().make_canvas(
        cfilename, drawing_width, drawing_height, this->properties.is_animated);
    this->_paint_all(*canvas);
    canvas->write(cfilename);
}

Void HybridFigure::_paint_all(CanvasInterface& canvas) const {
    // Project the bounding box onto the canvas
    double xl = numeric_cast<double>(bounds[variables.x()].lower_bound());
    double xu = numeric_cast<double>(bounds[variables.x()].upper_bound());
    double yl = numeric_cast<double>(bounds[variables.y()].lower_bound());
    double yu = numeric_cast<double>(bounds[variables.y()].upper_bound());

    canvas.initialise(variables.x().name(), variables.y().name(), xl, xu, yl, yu);

    // Draw shapes
    for (SizeType i = 0; i != objects.size(); ++i) {
        const HybridDrawableInterface& shape = *objects[i].shape_ptr;
        set_properties(canvas, objects[i].properties);
        shape.draw(canvas, this->locations, this->variables);
    }

    canvas.finalise();
}

} // namespace Ariadne

