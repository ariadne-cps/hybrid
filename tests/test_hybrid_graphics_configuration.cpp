/***************************************************************************
 *            tests/test_hybrid_graphics_configuration.cpp
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

/** @file tests/test_hybrid_graphics_configuration.cpp
 *  @brief Exercise hybrid graphics configuration behavior.
 */

// Behavior-based coverage of figure configuration and graphic-object
// composition.  No renderer or file output is needed for these contracts.
#include "ariadne-testing.hpp"
#include "hybrid/hybrid_graphics.hpp"
#include "hybrid/hybrid_set.hpp"

using namespace Ariadne;

class TestHybridGraphicsConfiguration : public Ariadne::Testing::ContextFixture {
public:
    using ContextFixture::ContextFixture;

    Void test_figure_properties() {
        RealVariable x("x"), y("y");
        DiscreteLocation loc(1);
        HybridFigure figure;

        ARIADNE_TESTING_PROVE(figure.objects.empty());
        figure.set_locations({loc});
        ARIADNE_TESTING_PROVE(figure.locations.contains(loc));

        Axes2d axes(-2.0,x,2.0,-3.0,y,3.0);
        figure.set_axes(axes);
        ARIADNE_TESTING_PROVE(figure.bounds.has_key(x));
        ARIADNE_TESTING_PROVE(figure.bounds.has_key(y));

        figure.set_bounds(x,-4.0,4.0);
        figure.set_bounds(y,ApproximateDoubleInterval(-5.0,5.0));
        Map<RealVariable,ApproximateDoubleInterval> new_bounds=figure.bounds;
        figure.set_bounds(new_bounds);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.bounds.size(),2u);
        figure.set_variables(x,y);
        ARIADNE_TESTING_PROVE(figure.variables.x()==x);
        ARIADNE_TESTING_PROVE(figure.variables.y()==y);

        figure.set_line_style(false);
        figure.set_fill_style(false);
        figure.set_line_width(3.0);
        figure.set_line_colour(red);
        figure.set_fill_colour(blue);
        ARIADNE_TESTING_PROVE(!figure.get_line_style());
        ARIADNE_TESTING_PROVE(!figure.get_fill_style());
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_line_width(),3.0);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_line_colour().red,red.red);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_fill_colour().blue,blue.blue);

        figure.set_fill_opacity(0.5);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_fill_colour().opacity,0.5);
        figure.set_line_colour(0.1,0.2,0.3);
        figure.set_fill_colour(0.4,0.5,0.6);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_line_colour().red,0.1);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_fill_colour().blue,0.6);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_fill_colour().opacity,0.5);

        // The public style operators delegate to the same configuration.
        figure << line_style(true) << fill_style(true)
               << line_width(2.0) << line_colour(black)
               << fill_colour(green) << fill_opacity(0.25);
        ARIADNE_TESTING_PROVE(figure.get_line_style());
        ARIADNE_TESTING_PROVE(figure.get_fill_style());
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_line_width(),2.0);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_line_colour().red,black.red);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.get_fill_colour().opacity,0.25);
    }

    Void test_graphic_objects() {
        DiscreteLocation location(1);
        RealVariable x("x"), y("y");
        RealSpace state_space({x,y});
        HybridExactBox shape(location,state_space,ExactBoxType({{0,1},{0,1}}));

        HybridFigure figure;
        ARIADNE_TESTING_PROVE(figure.objects.empty());
        figure.draw(shape);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.objects.size(),1u);
        ARIADNE_TESTING_PROVE(figure.objects[0].shape_ptr==&shape);

        draw(figure,shape);
        figure << shape;
        draw(figure,blue,shape);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.objects.size(),4u);

        // No-op draw does not add anything.
        draw(figure);
        ARIADNE_TESTING_EXPECT_EQUAL(figure.objects.size(),4u);
        figure.clear();
        ARIADNE_TESTING_PROVE(figure.objects.empty());
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_graphics_configuration");
    ARIADNE_TESTING_ADD_TEST(TestHybridGraphicsConfiguration, test_figure_properties);
    ARIADNE_TESTING_ADD_TEST(TestHybridGraphicsConfiguration, test_graphic_objects);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
