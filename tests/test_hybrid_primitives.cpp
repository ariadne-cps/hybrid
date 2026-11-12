/***************************************************************************
 *            tests/test_hybrid_primitives.cpp
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

/** @file tests/test_hybrid_primitives.cpp
 *  @brief Exercise hybrid primitives behavior.
 */

// Point construction, named-coordinate permutations and validated images.
#include <memory>
#include <sstream>
#include <string>

#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_set.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

namespace Ariadne {
HybridExactBox under_approximation(const HybridRealBox&);
HybridExactBox over_approximation(const HybridRealBox&);
} // namespace Ariadne

class TestHybridPrimitives : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_points_real() { test_points<Real>(); }
    Void test_points_approximation() { test_points<FloatDPApproximation>(); }
    Void test_boxes_exact() { test_boxes<ExactIntervalType>(); }
    Void test_boxes_upper() { test_boxes<UpperIntervalType>(); }

    template<class X>
    X coordinate(Int value) {
        if constexpr (Same<X, Real>)
            return Real(value);
        else
            return X(value, dp);
    }
    Void check_coordinate(const Real& value, Int expected) {
        auto enclosure = value.get(dp);
        ARIADNE_TESTING_EXPECT_EQUAL(enclosure.lower_raw().get_d(), static_cast<double>(expected));
        ARIADNE_TESTING_EXPECT_EQUAL(enclosure.upper_raw().get_d(), static_cast<double>(expected));
    }
    Void check_coordinate(const FloatDPApproximation& value, Int expected) {
        ARIADNE_TESTING_EXPECT_EQUAL(value.raw().get_d(), static_cast<double>(expected));
    }
    template<class X>
    X named_coordinate(const HybridPoint<X>& point, const RealVariable& variable) {
        const auto values = point.values();
        return values[variable];
    }
    template<class X>
    Void test_points() {
        const DiscreteLocation loc(1);
        const RealVariable x("x"), y("y");
        const RealSpace space({x, y});
        const X a = coordinate<X>(1), b = coordinate<X>(2);
        HybridPoint<X> empty;
        ARIADNE_TESTING_EXPECT_EQUAL(empty.dimension(), 0u);
        ARIADNE_TESTING_PROVE(empty.values().empty());
        Point<X> coordinates({a, b});
        HybridPoint<X> direct(loc, space, coordinates);
        ARIADNE_TESTING_EXPECT_EQUAL(direct.dimension(), 2u);
        check_coordinate(named_coordinate(direct, x), 1);
        direct.point()[1] = coordinate<X>(3);
        const HybridPoint<X>& immutable = direct;
        check_coordinate(immutable.point()[1], 3);
        HybridPoint<X> mapped(loc, Map<RealVariable, X>{
                                       {x, a},
                                       {y, b}
        });
        check_coordinate(named_coordinate(mapped, y), 2);
        const List<Assignment<RealVariable, X>> assignments{
            {x, a},
            {y, b}
        };
        HybridPoint<X> from_list(loc, assignments);
        check_coordinate(named_coordinate(from_list, x), 1);
        HybridPoint<X> from_initializer(loc, InitializerList<Assignment<RealVariable, X>>{
                                                 {x, a},
                                                 {y, b}
        });
        check_coordinate(named_coordinate(from_initializer, y), 2);
        HybridBasicSet<Point<X>> labelled(loc, LabelledSet<Point<X>>(space, coordinates));
        ARIADNE_TESTING_EXPECT_EQUAL(labelled.dimension(), 2u);
        check_coordinate(labelled.euclidean_set()[0], 1);
    }
    template<class IVL>
    Void check_interval(const IVL& interval, Int lower, Int upper) {
        if constexpr (Same<IVL, RealInterval>) {
            check_coordinate(interval.lower_bound(), lower);
            check_coordinate(interval.upper_bound(), upper);
        } else {
            ARIADNE_TESTING_EXPECT_EQUAL(interval.lower_bound().get_d(), static_cast<double>(lower));
            ARIADNE_TESTING_EXPECT_EQUAL(interval.upper_bound().get_d(), static_cast<double>(upper));
        }
    }
    template<class IVL>
    Void test_boxes() {
        using UB = typename IVL::UpperBoundType;
        const DiscreteLocation loc(1);
        const RealVariable x("x"), y("y");
        const RealSpace space({x, y}), reverse({y, x});
        const Box<IVL> euclidean({
            {0, 1},
            {2, 3}
        });
        const LabelledSet<Box<IVL>> labelled(space, euclidean);
        HybridBox<IVL> direct(loc, space, euclidean);
        HybridBox<IVL> from_labelled(loc, labelled);
        VariablesBox<IVL> variables(space, euclidean);
        HybridBox<IVL> from_variables(loc, variables);
        List<VariableInterval<UB>> intervals{
            {x, euclidean[0]},
            {y, euclidean[1]}
        };
        HybridBox<IVL> from_list(loc, intervals);
        HybridBox<IVL> from_initializer(loc, InitializerList<VariableInterval<UB>>{
                                                 {x, euclidean[0]},
                                                 {y, euclidean[1]}
        });
        ARIADNE_TESTING_EXPECT_EQUAL(direct.dimension(), 2u);
        ARIADNE_TESTING_PROVE(direct.variables().contains(y));
        check_interval(from_list[x], 0, 1);
        check_interval(from_initializer[y], 2, 3);
        check_interval(from_variables.euclidean_set(space)[0], 0, 1);
        check_interval(from_labelled.euclidean_set(reverse)[0], 2, 3);
        HybridBox<IVL> empty(loc, List<VariableInterval<UB>>{});
        ARIADNE_TESTING_EXPECT_EQUAL(empty.dimension(), 0u);
        HybridBoxes<IVL> collection;
        collection.insert(direct);
        collection.insert(DiscreteLocation(2), labelled);
        ARIADNE_TESTING_EXPECT_EQUAL(collection.locations().size(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(collection.space(loc).dimension(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(collection.continuous_set(loc).dimension(), 2u);
        check_interval(collection.euclidean_set(loc)[0], 0, 1);
        ARIADNE_TESTING_PROVE(direct.bounding_box().location() == loc);
        ARIADNE_TESTING_PROVE(direct.bounding_boxes().has_location(loc));
        std::ostringstream text;
        text << static_cast<const HybridBasicSet<Box<IVL>>&>(direct);
        ARIADNE_TESTING_PROVE(text.str().find("Box") != std::string::npos);
        ARIADNE_TESTING_PROVE(text.str().find("x") != std::string::npos);
    }
    Void test_real_box_enclosures() {
        DiscreteLocation loc(1);
        RealVariable x("x"), y("y");
        RealSpace space({x, y});
        RealBox euclidean({
            {0, 1},
            {2, 3}
        });
        HybridRealBox box(loc, space, euclidean);
        check_interval(box[x], 0, 1);
        check_interval(box.euclidean_set(RealSpace({y, x}))[0], 2, 3);
        HybridExactBox inner = under_approximation(box), outer = over_approximation(box);
        ARIADNE_TESTING_PROVE(inner.location() == loc);
        ARIADNE_TESTING_PROVE(outer.location() == loc);
        check_interval(inner.euclidean_set()[0], 0, 1);
        check_interval(outer.euclidean_set()[1], 2, 3);
        std::ostringstream text;
        text << static_cast<const HybridBasicSet<RealBox>&>(box);
        ARIADNE_TESTING_PROVE(text.str().find("RealBox") != std::string::npos);
    }
    Void test_validated_image() {
        DiscreteLocation loc(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace space({x, y});
        ValidatedConstrainedImageSet euclidean(ExactBoxType({
            {0, 1},
            {2, 3}
        }));
        HybridValidatedConstrainedImageSet image(loc, space, euclidean);
        ARIADNE_TESTING_PROVE(image.variables(loc).contains(x));
        ARIADNE_TESTING_PROVE(image.locations().contains(loc));
        ARIADNE_TESTING_EXPECT_EQUAL(image.bounding_box().locations().size(), 1u);
        std::unique_ptr<HybridValidatedConstrainedImageSet> copy(image.clone());
        ARIADNE_TESTING_PROVE(copy->locations().contains(loc));
        const ValidatedHybridLocatedSetInterface& view = image;
        std::unique_ptr<ValidatedHybridLocatedSetInterface> cloned(view.clone());
        ARIADNE_TESTING_PROVE(cloned->variables(loc).contains(y));
        const HybridSetInterfaceBase& root = image;
        ARIADNE_TESTING_EXPECT_EQUAL(root.euclidean_set(loc, space).dimension(), 2u);
        HybridExactBox disjoint(loc, space, ExactBoxType({
                                                {4, 5},
                                                {6, 7}
        }));
        HybridExactBox absent(other, space, ExactBoxType({
                                                {4, 5},
                                                {6, 7}
        }));
        ARIADNE_TESTING_PROVE(definitely(image.separated(disjoint)));
        ARIADNE_TESTING_PROVE(definitely(image.separated(absent)));
        HybridExactBox enclosing(loc, space, ExactBoxType({
                                                 {-1, 2},
                                                 { 1, 4}
        }));
        ARIADNE_TESTING_PROVE(definitely(image.overlaps(enclosing)));
        ARIADNE_TESTING_PROVE(!definitely(image.overlaps(absent)));
        HybridExactBoxes boxes;
        boxes.insert(enclosing);
        ARIADNE_TESTING_PROVE(definitely(image.inside(boxes)));
        HybridExactBoxes no_boxes;
        ARIADNE_TESTING_PROVE(!definitely(image.inside(no_boxes)));
        std::ostringstream text;
        image._write(text);
        text << image;
        ARIADNE_TESTING_PROVE(!text.str().empty());
        ARIADNE_TESTING_EXPECT_EXCEPTION(image.variables(other));
        ARIADNE_TESTING_EXPECT_EXCEPTION(root.euclidean_set(other, space));
        ARIADNE_TESTING_EXPECT_EXCEPTION(root.euclidean_set(loc, RealSpace({y, x})));
    }
    Void test_equality_and_list_iteration() {
        DiscreteLocation first(1), second(2), third(3), fourth(4);
        RealVariable x("x"), y("y");
        RealSpace space({x});
        ExactBoxType unit({
            {0, 1}
        }),
            next_box({{1, 2}});
        HybridExactBox original(first, space, unit), same_box(first, space, unit);
        HybridExactBox other_location(second, space, unit);
        ARIADNE_TESTING_PROVE(definitely(original == same_box));
        ARIADNE_TESTING_PROVE(!definitely(original == other_location));
        HybridExactBox other_space(first, RealSpace({y}), unit);
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(original == other_space));
        HybridListSet<ExactBoxType> list;
        list.insert(first, space);
        list.insert(third, space);
        list.adjoin(second, space, unit);
        list.adjoin(second, space, next_box);
        list.adjoin(fourth, space, unit);
        ARIADNE_TESTING_EXPECT_EXCEPTION(list.adjoin(second, RealSpace({y}), unit));
        auto begin = list.begin();
        auto shifted = begin;
        ARIADNE_TESTING_PROVE(begin == shifted);
        ++shifted;
        ARIADNE_TESTING_PROVE(begin != shifted);
        ARIADNE_TESTING_PROVE((*begin).location() == (*shifted).location());
        ++shifted;
        ARIADNE_TESTING_PROVE(begin != shifted);
        ARIADNE_TESTING_PROVE((*shifted).location() == fourth);
        ++shifted;
        ARIADNE_TESTING_PROVE(shifted == list.end());
        HybridListSet<ExactBoxType> empty;
        ARIADNE_TESTING_PROVE(empty.begin() == empty.end());
        ARIADNE_TESTING_EXPECT_EQUAL(list.size(), 3u);
    }

    Void test_cell_and_curve_descriptions() {
        DiscreteLocation loc(1);
        RealVariable x("x"), y("y");
        RealSpace space({x, y});
        MonolithicHybridSpace hspace;
        hspace.new_location(loc, space);
        HybridPaving paving{HybridGrid(hspace)};
        HybridExactBoxes boxes;
        boxes.insert(loc, space, ExactBoxType({
                                     {0, 1},
                                     {0, 1}
        }));
        paving.adjoin_outer_approximation(boxes, 1u);
        HybridGridCell cell = *paving.begin();
        ARIADNE_TESTING_EXPECT_EQUAL(cell.dimension(), 2u);
        ARIADNE_TESTING_PROVE(cell.box().location() == loc);
        std::ostringstream cell_text;
        cell_text << cell;
        ARIADNE_TESTING_PROVE(cell_text.str().find("GridCell") != std::string::npos);
        InterpolatedCurve curve(Point<FloatDPApproximation>({0.0_x, 0.0_x}, dp),
                                Point<FloatDPApproximation>({1.0_x, 1.0_x}, dp));
        HybridBasicSet<InterpolatedCurve> path(loc, space, curve);
        std::ostringstream curve_text;
        curve_text << path;
        ARIADNE_TESTING_PROVE(curve_text.str().find("InterpolatedCurve") != std::string::npos);
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_primitives");
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_points_real);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_points_approximation);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_boxes_exact);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_boxes_upper);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_real_box_enclosures);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_validated_image);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_cell_and_curve_descriptions);
    ARIADNE_TESTING_ADD_TEST(TestHybridPrimitives, test_equality_and_list_iteration);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
