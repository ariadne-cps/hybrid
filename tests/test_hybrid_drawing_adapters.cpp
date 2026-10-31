// Semantic rendering-adapter tests with an in-memory CanvasInterface.
// No files, graphical backend or display server are needed.
#include "macro/testing.hpp"
#include "rendering/rendering_interface.hpp"
#include "hybrid/hybrid_graphics.hpp"
#include "hybrid/hybrid_set.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_paving.hpp"

using namespace Ariadne;

class RecordingCanvas final : public CanvasInterface {
public:
    SizeType initialisations=0u;
    SizeType finalisations=0u;
    SizeType boundaries=0u;
    SizeType primitives=0u;
    SizeType styles=0u;
    StringType horizontal_axis;
    StringType vertical_axis;
    double x_lower=0.0;
    double x_upper=0.0;

    Void initialise(StringType x, StringType y,
                    double lx, double ux, double ly, double uy) override {
        static_cast<void>(ly);
        static_cast<void>(uy);
        ++initialisations;
        horizontal_axis=x;
        vertical_axis=y;
        x_lower=lx;
        x_upper=ux;
    }
    Void finalise() override { ++finalisations; }
    Void write(const char* filename) const override { static_cast<void>(filename); }

    Void move_to(double x, double y) override {
        static_cast<void>(x); static_cast<void>(y); ++primitives;
    }
    Void line_to(double x, double y) override {
        static_cast<void>(x); static_cast<void>(y); ++primitives;
    }
    Void circle(double x, double y, double radius) override {
        static_cast<void>(x); static_cast<void>(y);
        static_cast<void>(radius); ++primitives;
    }
    Void dot(double x, double y) override {
        static_cast<void>(x); static_cast<void>(y); ++primitives;
    }
    Void stroke() override { ++primitives; }
    Void fill() override { ++primitives; }
    Void fill_boundary(List<Point2d> const& boundary) override {
        static_cast<void>(boundary);
        ++boundaries;
    }
    Void set_line_width(double width) override {
        static_cast<void>(width); ++styles;
    }
    Void set_line_colour(double red_component, double green_component, double blue_component) override {
        static_cast<void>(red_component); static_cast<void>(green_component);
        static_cast<void>(blue_component); ++styles;
    }
    Void set_fill_opacity(double opacity) override {
        static_cast<void>(opacity); ++styles;
    }
    Void set_fill_colour(double red_component, double green_component, double blue_component) override {
        static_cast<void>(red_component); static_cast<void>(green_component);
        static_cast<void>(blue_component); ++styles;
    }
};

class TestHybridDrawingAdapters {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_location_selection());
        ARIADNE_TEST_CALL(test_box_set_and_box_collection());
        ARIADNE_TEST_CALL(test_figure_paint());
    }

    Void test_location_selection() {
        DiscreteLocation q(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x,y});
        Variables2d axes(x,y);
        HybridExactBox shape(q,spc,ExactBoxType({{0,1},{0,1}}));

        RecordingCanvas canvas;
        Set<DiscreteLocation> excluded={other};
        Set<DiscreteLocation> selected={q};
        Set<DiscreteLocation> all;

        // A nonmatching location must not reach the Euclidean renderer.
        shape.draw(canvas,excluded,axes);
        ARIADNE_TEST_EQUALS(canvas.boundaries,0u);
        shape.draw(canvas,selected,axes);
        ARIADNE_TEST_EQUALS(canvas.boundaries,1u);
        shape.draw(canvas,all,axes);
        ARIADNE_TEST_EQUALS(canvas.boundaries,2u);

        // The HybridBasicSet adapter iterates explicit location selections;
        // an empty selection draws nothing, unlike HybridBox::draw.
        HybridBasicSet<ExactBoxType> const& base=shape;
        base.draw(canvas,excluded,axes);
        ARIADNE_TEST_EQUALS(canvas.boundaries,2u);
        base.draw(canvas,selected,axes);
        ARIADNE_TEST_EQUALS(canvas.boundaries,3u);
        base.draw(canvas,all,axes);
        ARIADNE_TEST_EQUALS(canvas.boundaries,3u);
    }

    Void test_box_set_and_box_collection() {
        DiscreteLocation q(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x,y});
        Variables2d axes(x,y);
        Set<DiscreteLocation> selected={q};
        Set<DiscreteLocation> excluded={other};
        Set<DiscreteLocation> all;

        HybridBoxSet set(q,RealVariablesBox({0<=x<=2,0<=y<=2}));
        RecordingCanvas set_canvas;
        set.draw(set_canvas,excluded,axes);
        ARIADNE_TEST_EQUALS(set_canvas.boundaries,0u);
        set.draw(set_canvas,selected,axes);
        ARIADNE_TEST_EQUALS(set_canvas.boundaries,1u);
        set.draw(set_canvas,all,axes);
        ARIADNE_TEST_EQUALS(set_canvas.boundaries,2u);

        HybridExactBoxes boxes;
        boxes.insert(q,spc,ExactBoxType({{0,1},{0,1}}));
        RecordingCanvas boxes_canvas;
        boxes.draw(boxes_canvas,excluded,axes);
        ARIADNE_TEST_EQUALS(boxes_canvas.boundaries,0u);
        boxes.draw(boxes_canvas,selected,axes);
        ARIADNE_TEST_EQUALS(boxes_canvas.boundaries,1u);
        boxes.draw(boxes_canvas,all,axes);
        ARIADNE_TEST_EQUALS(boxes_canvas.boundaries,2u);
    }

    Void test_figure_paint() {
        DiscreteLocation q(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x,y});
        HybridExactBox shape(q,spc,ExactBoxType({{0,1},{0,1}}));

        HybridFigure fig;
        fig.set_bounds(x,-2.0,2.0);
        fig.set_bounds(y,-3.0,3.0);
        fig.set_variables(x,y);
        fig.set_locations({q});
        fig.draw(shape);

        RecordingCanvas painted;
        fig._paint_all(painted);
        ARIADNE_TEST_EQUALS(painted.initialisations,1u);
        ARIADNE_TEST_EQUALS(painted.finalisations,1u);
        ARIADNE_TEST_ASSERT(painted.horizontal_axis=="x");
        ARIADNE_TEST_ASSERT(painted.vertical_axis=="y");
        ARIADNE_TEST_ASSERT(painted.x_lower<painted.x_upper);
        ARIADNE_TEST_ASSERT(painted.styles>0u);
        ARIADNE_TEST_EQUALS(painted.boundaries,1u);

        // The figure still initialises and finalises the canvas when its
        // selected location excludes the registered drawable.
        fig.set_locations({other});
        RecordingCanvas filtered;
        fig._paint_all(filtered);
        ARIADNE_TEST_EQUALS(filtered.initialisations,1u);
        ARIADNE_TEST_EQUALS(filtered.finalisations,1u);
        ARIADNE_TEST_EQUALS(filtered.boundaries,0u);

        // An empty object list does not issue drawing operations.
        fig.clear();
        RecordingCanvas empty;
        fig._paint_all(empty);
        ARIADNE_TEST_EQUALS(empty.initialisations,1u);
        ARIADNE_TEST_EQUALS(empty.finalisations,1u);
        ARIADNE_TEST_EQUALS(empty.boundaries,0u);
        ARIADNE_TEST_EQUALS(empty.styles,0u);
    }
};

Int main() {
    TestHybridDrawingAdapters().test();
    return ARIADNE_TEST_FAILURES;
}
