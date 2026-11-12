/***************************************************************************
 *            tests/test_hybrid_drawing_adapters.cpp
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

/** @file tests/test_hybrid_drawing_adapters.cpp
 *  @brief Exercise hybrid drawing adapters behavior.
 */

// Semantic rendering-adapter tests with an in-memory CanvasInterface.
// No files, graphical backend or display server are needed.
#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_graphics.hpp"
#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_set.hpp"
#include "rendering/rendering_interface.hpp"
#include "rendering/rendering_manager.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

class RecordingCanvas final : public CanvasInterface {
  public:
    mutable SizeType writes = 0u;
    mutable StringType written_filename;
    SizeType initialisations = 0u;
    SizeType finalisations = 0u;
    SizeType boundaries = 0u;
    SizeType primitives = 0u;
    SizeType styles = 0u;
    StringType horizontal_axis;
    StringType vertical_axis;
    double x_lower = 0.0;
    double x_upper = 0.0;

    Void initialise(StringType x, StringType y,
                    double lx, double ux, double ly, double uy) override {
        static_cast<void>(ly);
        static_cast<void>(uy);
        ++initialisations;
        horizontal_axis = x;
        vertical_axis = y;
        x_lower = lx;
        x_upper = ux;
    }
    Void finalise() override { ++finalisations; }
    Void write(const char* filename) const override {
        ++writes;
        written_filename = filename;
    }

    Void move_to(double x, double y) override {
        static_cast<void>(x);
        static_cast<void>(y);
        ++primitives;
    }
    Void line_to(double x, double y) override {
        static_cast<void>(x);
        static_cast<void>(y);
        ++primitives;
    }
    Void circle(double x, double y, double radius) override {
        static_cast<void>(x);
        static_cast<void>(y);
        static_cast<void>(radius);
        ++primitives;
    }
    Void dot(double x, double y) override {
        static_cast<void>(x);
        static_cast<void>(y);
        ++primitives;
    }
    Void stroke() override { ++primitives; }
    Void fill() override { ++primitives; }
    Void fill_boundary(const List<Point2d>& boundary) override {
        static_cast<void>(boundary);
        ++boundaries;
    }
    Void set_line_width(double width) override {
        static_cast<void>(width);
        ++styles;
    }
    Void set_line_colour(double red_component, double green_component, double blue_component) override {
        static_cast<void>(red_component);
        static_cast<void>(green_component);
        static_cast<void>(blue_component);
        ++styles;
    }
    Void set_fill_opacity(double opacity) override {
        static_cast<void>(opacity);
        ++styles;
    }
    Void set_fill_colour(double red_component, double green_component, double blue_component) override {
        static_cast<void>(red_component);
        static_cast<void>(green_component);
        static_cast<void>(blue_component);
        ++styles;
    }
};

class RecordingBackend final : public RenderingBackendInterface {
  public:
    SharedPointer<RecordingCanvas> canvas;
    mutable Nat width = 0u, height = 0u;
    mutable Bool animated = false;
    mutable StringType filename;
    explicit RecordingBackend(SharedPointer<RecordingCanvas> recording) : canvas(recording) {}
    SharedPointer<CanvasInterface> make_canvas(const char* name, Nat w, Nat h, Bool animation) const override {
        filename = name;
        width = w;
        height = h;
        animated = animation;
        return canvas;
    }
};

// A backend change is process-wide; restore it even if painting throws.
class BackendScope {
    RenderingBackend _previous;

  public:
    explicit BackendScope(RenderingBackend backend)
        : _previous(RenderingManager::instance().backend()) {
        RenderingManager::instance().set_backend(backend);
    }
    ~BackendScope() { RenderingManager::instance().set_backend(_previous); }
};

class TestHybridDrawingAdapters : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_location_selection() {
        DiscreteLocation q(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x, y});
        Variables2d axes(x, y);
        HybridExactBox shape(q, spc, ExactBoxType({
                                         {0, 1},
                                         {0, 1}
        }));

        RecordingCanvas canvas;
        Set<DiscreteLocation> excluded = {other};
        Set<DiscreteLocation> selected = {q};
        Set<DiscreteLocation> all;

        // A nonmatching location must not reach the Euclidean renderer.
        shape.draw(canvas, excluded, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries, 0u);
        shape.draw(canvas, selected, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries, 1u);
        shape.draw(canvas, all, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries, 2u);

        // The base adapter follows the same empty-means-all contract.
        const HybridBasicSet<ExactBoxType>& base = shape;
        base.draw(canvas, excluded, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries, 2u);
        base.draw(canvas, selected, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries, 3u);
        base.draw(canvas, all, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries, 4u);
    }

    Void test_box_set_and_box_collection() {
        DiscreteLocation q(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x, y});
        Variables2d axes(x, y);
        Set<DiscreteLocation> selected = {q};
        Set<DiscreteLocation> excluded = {other};
        Set<DiscreteLocation> all;

        HybridBoxSet set(q, RealVariablesBox({0 <= x <= 2, 0 <= y <= 2}));
        RecordingCanvas set_canvas;
        set.draw(set_canvas, excluded, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(set_canvas.boundaries, 0u);
        set.draw(set_canvas, selected, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(set_canvas.boundaries, 1u);
        set.draw(set_canvas, all, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(set_canvas.boundaries, 2u);

        HybridExactBoxes boxes;
        boxes.insert(q, spc, ExactBoxType({
                                 {0, 1},
                                 {0, 1}
        }));
        RecordingCanvas boxes_canvas;
        boxes.draw(boxes_canvas, excluded, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(boxes_canvas.boundaries, 0u);
        boxes.draw(boxes_canvas, selected, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(boxes_canvas.boundaries, 1u);
        boxes.draw(boxes_canvas, all, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(boxes_canvas.boundaries, 2u);
    }

    template<class Shape>
    Void check_location_filter(const Shape& shape,
                               DiscreteLocation loc, const Variables2d& axes) {
        RecordingCanvas canvas;
        shape.draw(canvas, Set<DiscreteLocation>{DiscreteLocation(99)}, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas.boundaries + canvas.primitives, 0u);
        shape.draw(canvas, Set<DiscreteLocation>{loc}, axes);
        const SizeType selected = canvas.boundaries + canvas.primitives;
        ARIADNE_TESTING_PROVE(selected > 0u);
        shape.draw(canvas, Set<DiscreteLocation>{}, axes);
        ARIADNE_TESTING_PROVE(canvas.boundaries + canvas.primitives > selected);
    }
    Void test_remaining_drawable_adapters() {
        DiscreteLocation loc(1);
        RealVariable x("x"), y("y");
        RealSpace space({x, y});
        Variables2d axes(x, y);
        HybridUpperBox upper(loc, space, UpperBoxType({
                                             {0, 1},
                                             {0, 1}
        }));
        HybridRealBox real(loc, space, RealBox({
                                           {0, 1},
                                           {0, 1}
        }));
        check_location_filter(upper, loc, axes);
        check_location_filter(real, loc, axes);
        check_location_filter(static_cast<const HybridBasicSet<UpperBoxType>&>(upper), loc, axes);
        HybridUpperBoxes boxes;
        boxes.insert(upper);
        check_location_filter(boxes, loc, axes);
        HybridUpperBoxes empty_boxes;
        RecordingCanvas empty;
        empty_boxes.draw(empty, {}, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.boundaries + empty.primitives, 0u);
        HybridBoundedConstraintSet bounded(loc, {0 <= x <= 1, 0 <= y <= 1});
        check_location_filter(bounded, loc, axes);
        HybridBoundedConstraintSet empty_constraints;
        empty_constraints.draw(empty, {}, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.boundaries + empty.primitives, 0u);
        HybridValidatedConstrainedImageSet image(loc, space,
                                                 ValidatedConstrainedImageSet(ExactBoxType({
                                                     {0, 1},
                                                     {0, 1}
        })));
        check_location_filter(image, loc, axes);
        MonolithicHybridSpace hspace;
        hspace.new_location(loc, space);
        HybridPaving paving{HybridGrid(hspace)};
        HybridExactBoxes exact_boxes;
        exact_boxes.insert(loc, space, ExactBoxType({
                                           {0, 1},
                                           {0, 1}
        }));
        paving.adjoin_outer_approximation(exact_boxes, 1u);
        check_location_filter(paving, loc, axes);
        HybridPaving empty_paving{HybridGrid(hspace)};
        empty_paving.draw(empty, {}, axes);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.boundaries + empty.primitives, 0u);
    }
    Void test_outer_approximation_adapters() {
        DiscreteLocation loc(1);
        RealVariable x("x"), y("y"), z("z");
        RealSpace space({x, y});
        MonolithicHybridSpace hspace;
        hspace.new_location(loc, space);
        HybridPaving paving{HybridGrid(hspace)};
        HybridExactBox exact(loc, space, ExactBoxType({
                                             {0, 1},
                                             {0, 1}
        }));
        HybridUpperBox upper(loc, space, UpperBoxType({
                                             {0, 1},
                                             {0, 1}
        }));
        const HybridBasicSet<ExactBoxType>& exact_base = exact;
        const HybridBasicSet<UpperBoxType>& upper_base = upper;
        exact_base.adjoin_outer_approximation_to(paving, 1u);
        const SizeType initial = paving.size();
        ARIADNE_TESTING_PROVE(initial > 0u);
        upper_base.adjoin_outer_approximation_to(paving, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(paving.size(), initial);
        MonolithicHybridSpace other_space;
        other_space.new_location(loc, RealSpace({x, z}));
        HybridPaving mismatched{HybridGrid(other_space)};
        ARIADNE_TESTING_EXPECT_EXCEPTION(exact_base.adjoin_outer_approximation_to(mismatched, 1u));
        ARIADNE_TESTING_EXPECT_EXCEPTION(upper_base.adjoin_outer_approximation_to(mismatched, 1u));
        ARIADNE_TESTING_PROVE(mismatched.is_empty());
    }

    Void test_figure_write_backend() {
        DiscreteLocation loc(1);
        RealVariable x("x"), y("y");
        RealSpace space({x, y});
        HybridExactBox shape(loc, space, ExactBoxType({
                                             {0, 1},
                                             {0, 1}
        }));
        auto canvas = std::make_shared<RecordingCanvas>();
        auto backend = std::make_shared<RecordingBackend>(canvas);
        BackendScope scope{RenderingBackend(backend)};
        HybridFigure figure;
        figure.set_axes(Axes2d(-2.0, x, 2.0, -3.0, y, 3.0));
        figure.draw(shape);
        figure.write("record-default.png");
        ARIADNE_TESTING_EXPECT_EQUAL(backend->width, 800u);
        ARIADNE_TESTING_EXPECT_EQUAL(backend->height, 800u);
        ARIADNE_TESTING_PROVE(!backend->animated);
        ARIADNE_TESTING_PROVE(backend->filename == "record-default.png");
        ARIADNE_TESTING_EXPECT_EQUAL(canvas->writes, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas->boundaries, 1u);
        figure.properties.is_animated = true;
        figure.write("record-sized.png", 320u, 240u);
        ARIADNE_TESTING_EXPECT_EQUAL(backend->width, 320u);
        ARIADNE_TESTING_EXPECT_EQUAL(backend->height, 240u);
        ARIADNE_TESTING_PROVE(backend->animated);
        ARIADNE_TESTING_PROVE(canvas->written_filename == "record-sized.png");
        ARIADNE_TESTING_EXPECT_EQUAL(canvas->writes, 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas->initialisations, 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas->finalisations, 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(canvas->boundaries, 2u);
        RecordingCanvas direct;
        Ariadne::draw(direct, Set<DiscreteLocation>{loc}, Variables2d(x, y), shape);
        ARIADNE_TESTING_EXPECT_EQUAL(direct.boundaries, 1u);
    }

    Void test_figure_paint() {
        DiscreteLocation q(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x, y});
        HybridExactBox shape(q, spc, ExactBoxType({
                                         {0, 1},
                                         {0, 1}
        }));

        HybridFigure fig;
        fig.set_bounds(x, -2.0, 2.0);
        fig.set_bounds(y, -3.0, 3.0);
        fig.set_variables(x, y);
        fig.set_locations({q});
        fig.draw(shape);

        RecordingCanvas painted;
        fig._paint_all(painted);
        ARIADNE_TESTING_EXPECT_EQUAL(painted.initialisations, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(painted.finalisations, 1u);
        ARIADNE_TESTING_PROVE(painted.horizontal_axis == "x");
        ARIADNE_TESTING_PROVE(painted.vertical_axis == "y");
        ARIADNE_TESTING_PROVE(painted.x_lower < painted.x_upper);
        ARIADNE_TESTING_PROVE(painted.styles > 0u);
        ARIADNE_TESTING_EXPECT_EQUAL(painted.boundaries, 1u);

        // The figure still initialises and finalises the canvas when its
        // selected location excludes the registered drawable.
        fig.set_locations({other});
        RecordingCanvas filtered;
        fig._paint_all(filtered);
        ARIADNE_TESTING_EXPECT_EQUAL(filtered.initialisations, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(filtered.finalisations, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(filtered.boundaries, 0u);

        // An empty object list does not issue drawing operations.
        fig.clear();
        RecordingCanvas empty;
        fig._paint_all(empty);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.initialisations, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.finalisations, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.boundaries, 0u);
        ARIADNE_TESTING_EXPECT_EQUAL(empty.styles, 0u);
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_drawing_adapters");
    ARIADNE_TESTING_ADD_TEST(TestHybridDrawingAdapters, test_location_selection);
    ARIADNE_TESTING_ADD_TEST(TestHybridDrawingAdapters, test_box_set_and_box_collection);
    ARIADNE_TESTING_ADD_TEST(TestHybridDrawingAdapters, test_figure_paint);
    ARIADNE_TESTING_ADD_TEST(TestHybridDrawingAdapters, test_figure_write_backend);
    ARIADNE_TESTING_ADD_TEST(TestHybridDrawingAdapters, test_remaining_drawable_adapters);
    ARIADNE_TESTING_ADD_TEST(TestHybridDrawingAdapters, test_outer_approximation_adapters);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
