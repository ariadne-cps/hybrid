// Behavior-based coverage of figure configuration and graphic-object
// composition.  No renderer or file output is needed for these contracts.
#include "macro/testing.hpp"
#include "hybrid/hybrid_graphics.hpp"
#include "hybrid/hybrid_set.hpp"

using namespace Ariadne;

class TestHybridGraphicsConfiguration {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_figure_properties());
        ARIADNE_TEST_CALL(test_graphic_objects());
    }

    Void test_figure_properties() {
        RealVariable x("x"), y("y");
        DiscreteLocation loc(1);
        HybridFigure figure;

        ARIADNE_TEST_ASSERT(figure.objects.empty());
        figure.set_locations({loc});
        ARIADNE_TEST_ASSERT(figure.locations.contains(loc));

        Axes2d axes(-2.0,x,2.0,-3.0,y,3.0);
        figure.set_axes(axes);
        ARIADNE_TEST_ASSERT(figure.bounds.has_key(x));
        ARIADNE_TEST_ASSERT(figure.bounds.has_key(y));

        figure.set_bounds(x,-4.0,4.0);
        figure.set_bounds(y,ApproximateDoubleInterval(-5.0,5.0));
        Map<RealVariable,ApproximateDoubleInterval> new_bounds=figure.bounds;
        figure.set_bounds(new_bounds);
        ARIADNE_TEST_EQUALS(figure.bounds.size(),2u);
        figure.set_variables(x,y);
        ARIADNE_TEST_ASSERT(figure.variables.x()==x);
        ARIADNE_TEST_ASSERT(figure.variables.y()==y);

        figure.set_line_style(false);
        figure.set_fill_style(false);
        figure.set_line_width(3.0);
        figure.set_line_colour(red);
        figure.set_fill_colour(blue);
        ARIADNE_TEST_ASSERT(!figure.get_line_style());
        ARIADNE_TEST_ASSERT(!figure.get_fill_style());
        ARIADNE_TEST_EQUALS(figure.get_line_width(),3.0);
        ARIADNE_TEST_EQUALS(figure.get_line_colour().red,red.red);
        ARIADNE_TEST_EQUALS(figure.get_fill_colour().blue,blue.blue);

        figure.set_fill_opacity(0.5);
        ARIADNE_TEST_EQUALS(figure.get_fill_colour().opacity,0.5);
        figure.set_line_colour(0.1,0.2,0.3);
        figure.set_fill_colour(0.4,0.5,0.6);
        ARIADNE_TEST_EQUALS(figure.get_line_colour().red,0.1);
        ARIADNE_TEST_EQUALS(figure.get_fill_colour().blue,0.6);
        ARIADNE_TEST_EQUALS(figure.get_fill_colour().opacity,0.5);

        // The public style operators delegate to the same configuration.
        figure << line_style(true) << fill_style(true)
               << line_width(2.0) << line_colour(black)
               << fill_colour(green) << fill_opacity(0.25);
        ARIADNE_TEST_ASSERT(figure.get_line_style());
        ARIADNE_TEST_ASSERT(figure.get_fill_style());
        ARIADNE_TEST_EQUALS(figure.get_line_width(),2.0);
        ARIADNE_TEST_EQUALS(figure.get_line_colour().red,black.red);
        ARIADNE_TEST_EQUALS(figure.get_fill_colour().opacity,0.25);
    }

    Void test_graphic_objects() {
        DiscreteLocation location(1);
        RealVariable x("x"), y("y");
        RealSpace state_space({x,y});
        HybridExactBox shape(location,state_space,ExactBoxType({{0,1},{0,1}}));

        HybridFigure figure;
        ARIADNE_TEST_ASSERT(figure.objects.empty());
        figure.draw(shape);
        ARIADNE_TEST_EQUALS(figure.objects.size(),1u);
        ARIADNE_TEST_ASSERT(figure.objects[0].shape_ptr==&shape);

        draw(figure,shape);
        figure << shape;
        draw(figure,blue,shape);
        ARIADNE_TEST_EQUALS(figure.objects.size(),4u);

        // No-op draw does not add anything.
        draw(figure);
        ARIADNE_TEST_EQUALS(figure.objects.size(),4u);
        figure.clear();
        ARIADNE_TEST_ASSERT(figure.objects.empty());
    }
};

Int main() {
    TestHybridGraphicsConfiguration().test();
    return ARIADNE_TEST_FAILURES;
}
