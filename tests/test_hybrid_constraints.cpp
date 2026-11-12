/***************************************************************************
 *            tests/test_hybrid_constraints.cpp
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

/** @file tests/test_hybrid_constraints.cpp
 *  @brief Exercise hybrid constraints behavior.
 */

// Behavioral tests for symbolic constraints, bounded hybrid sets and
// intersection of location-indexed boxes with partially specified constraints.
#include <memory>
#include <sstream>
#include <string>

#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_constraint_set.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_set.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestHybridConstraints : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_constraints() {
        DiscreteLocation q(1), other(2), absent(3);
        RealVariable x("x");
        RealSpace space({x});
        List<ContinuousPredicate> predicates({x >= 0, x <= 2});
        RealExpressionConstraintSet symbolic(predicates);

        HybridConstraintSet empty;
        ARIADNE_TESTING_PROVE(empty.locations().empty());
        ARIADNE_TESTING_PROVE(!empty.has_location(q));

        HybridConstraintSet from_predicates(q, predicates);
        HybridConstraintSet from_symbolic(q, symbolic);
        Set<DiscreteLocation> both = {q, other};
        HybridConstraintSet multi(both, symbolic);
        ARIADNE_TESTING_EXPECT_EQUAL(multi.locations().size(), 2u);
        ARIADNE_TESTING_PROVE(multi.has_location(other));
        ARIADNE_TESTING_PROVE(from_predicates.has_location(q));
        ARIADNE_TESTING_PROVE(from_symbolic.variables(q).contains(x));
        ARIADNE_TESTING_EXPECT_EQUAL(from_symbolic.euclidean_set(q, space).dimension(), 1u);
        ARIADNE_TESTING_PROVE(from_symbolic.continuous_set(q).variables().contains(x));

        HybridConstraintSet appended;
        appended.adjoin(q, predicates);
        appended.adjoin(other, symbolic);
        ARIADNE_TESTING_EXPECT_EQUAL(appended.locations().size(), 2u);
        std::unique_ptr<HybridConstraintSet> clone(appended.clone());
        ARIADNE_TESTING_EXPECT_EQUAL(clone->locations().size(), 2u);

        HybridExactBox known(q, space, ExactBoxType({
                                           {0, 1}
        }));
        HybridExactBox unknown(absent, space, ExactBoxType({
                                                  {0, 1}
        }));
        // The unknown location denotes an empty slice.
        ARIADNE_TESTING_PROVE(definitely(from_symbolic.separated(unknown)));
        static_cast<void>(from_symbolic.overlaps(known));
        static_cast<void>(from_symbolic.covers(known));
        static_cast<void>(from_symbolic.separated(known));
        static_cast<void>(from_symbolic.overlaps(unknown));
        static_cast<void>(from_symbolic.covers(unknown));

        std::ostringstream output;
        from_symbolic._write(output);
        ARIADNE_TESTING_PROVE(output.str().find("HybridConstraintSet") != std::string::npos);
    }

    Void test_bounded_sets() {
        DiscreteLocation q(1), other(2), absent(3);
        RealVariable x("x");
        RealSpace space({x});
        RealExpressionBoundedConstraintSet symbolic({0 <= x <= 2});

        HybridBoundedConstraintSet empty;
        ARIADNE_TESTING_PROVE(empty.locations().empty());
        HybridBoundedConstraintSet from_symbolic(q, symbolic);
        HybridBoundedConstraintSet from_intervals(q, {0 <= x <= 2});
        HybridBoundedConstraintSet with_predicate(q, {0 <= x <= 2}, {x >= 0});
        RealVariablesBox raw({0 <= x <= 2});
        HybridBoundedConstraintSet from_box(q, raw);
        Set<DiscreteLocation> both = {q, other};
        HybridBoundedConstraintSet multiple(both, symbolic);

        ARIADNE_TESTING_EXPECT_EQUAL(multiple.locations().size(), 2u);
        ARIADNE_TESTING_PROVE(from_symbolic.variables(q).contains(x));
        ARIADNE_TESTING_PROVE(from_intervals.variables(q).contains(x));
        ARIADNE_TESTING_PROVE(with_predicate.variables(q).contains(x));
        ARIADNE_TESTING_PROVE(from_box.variables(q).contains(x));
        ARIADNE_TESTING_EXPECT_EQUAL(from_symbolic.euclidean_set(q, space).dimension(), 1u);
        ARIADNE_TESTING_PROVE(from_symbolic.continuous_set(q).variables().contains(x));
        ARIADNE_TESTING_PROVE(are_same(from_symbolic.location(), q));
        ARIADNE_TESTING_PROVE(from_symbolic.bounding_box().has_location(q));

        HybridBoundedConstraintSet appended;
        appended.adjoin(q, symbolic);
        appended.adjoin(other, symbolic);
        ARIADNE_TESTING_EXPECT_EQUAL(appended.locations().size(), 2u);
        std::unique_ptr<HybridBoundedConstraintSet> clone(appended.clone());
        ARIADNE_TESTING_EXPECT_EQUAL(clone->locations().size(), 2u);

        HybridExactBox known(q, space, ExactBoxType({
                                           {0, 1}
        }));
        HybridExactBox unknown(absent, space, ExactBoxType({
                                                  {0, 1}
        }));
        ARIADNE_TESTING_PROVE(definitely(from_symbolic.separated(unknown)));
        static_cast<void>(from_symbolic.overlaps(known));
        static_cast<void>(from_symbolic.covers(known));
        static_cast<void>(from_symbolic.separated(known));
        static_cast<void>(from_symbolic.overlaps(unknown));
        static_cast<void>(from_symbolic.covers(unknown));
        HybridExactBoxes boxes;
        boxes.insert(q, space, ExactBoxType({
                                   {0, 2}
        }));
        static_cast<void>(from_symbolic.inside(boxes));

        std::ostringstream output;
        from_symbolic._write(output);
        ARIADNE_TESTING_PROVE(output.str().find("HybridBoundedConstraintSet") != std::string::npos);
    }

    Void test_intersection_with_missing_constraint() {
        DiscreteLocation constrained(1), unconstrained(2);
        RealVariable x("x");
        RealSpace space({x});
        RealVariablesBox real_box({0 <= x <= 2});
        Set<DiscreteLocation> locations = {constrained, unconstrained};
        HybridBoxesSet bounds(locations, real_box);

        List<ContinuousPredicate> predicates({x >= 0});
        HybridConstraintSet restrictions(constrained, predicates);
        HybridBoundedConstraintSet intersected = intersection(bounds, restrictions);
        ARIADNE_TESTING_EXPECT_EQUAL(intersected.locations().size(), 2u);
        ARIADNE_TESTING_PROVE(intersected.locations().contains(constrained));
        ARIADNE_TESTING_PROVE(intersected.locations().contains(unconstrained));
        ARIADNE_TESTING_EXPECT_EQUAL(intersected.euclidean_set(unconstrained, space).dimension(), 1u);
        ARIADNE_TESTING_PROVE(intersected.bounding_box().has_location(unconstrained));
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_constraints");
    ARIADNE_TESTING_ADD_TEST(TestHybridConstraints, test_constraints);
    ARIADNE_TESTING_ADD_TEST(TestHybridConstraints, test_bounded_sets);
    ARIADNE_TESTING_ADD_TEST(TestHybridConstraints, test_intersection_with_missing_constraint);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
