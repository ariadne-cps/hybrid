// Behavioral tests for symbolic constraints, bounded hybrid sets and
// intersection of location-indexed boxes with partially specified constraints.
#include <memory>
#include <sstream>
#include <string>

#include "macro/testing.hpp"
#include "hybrid/hybrid_constraint_set.hpp"
#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_set.hpp"

using namespace Ariadne;

class TestHybridConstraints {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_constraints());
        ARIADNE_TEST_CALL(test_bounded_sets());
        ARIADNE_TEST_CALL(test_intersection_with_missing_constraint());
    }

    Void test_constraints() {
        DiscreteLocation q(1), other(2), absent(3);
        RealVariable x("x");
        RealSpace space({x});
        List<ContinuousPredicate> predicates({x>=0,x<=2});
        RealExpressionConstraintSet symbolic(predicates);

        HybridConstraintSet empty;
        ARIADNE_TEST_ASSERT(empty.locations().empty());
        ARIADNE_TEST_ASSERT(!empty.has_location(q));

        HybridConstraintSet from_predicates(q,predicates);
        HybridConstraintSet from_symbolic(q,symbolic);
        Set<DiscreteLocation> both={q,other};
        HybridConstraintSet multi(both,symbolic);
        ARIADNE_TEST_EQUALS(multi.locations().size(),2u);
        ARIADNE_TEST_ASSERT(multi.has_location(other));
        ARIADNE_TEST_ASSERT(from_predicates.has_location(q));
        ARIADNE_TEST_ASSERT(from_symbolic.variables(q).contains(x));
        ARIADNE_TEST_EQUALS(from_symbolic.euclidean_set(q,space).dimension(),1u);
        ARIADNE_TEST_ASSERT(from_symbolic.continuous_set(q).variables().contains(x));

        HybridConstraintSet appended;
        appended.adjoin(q,predicates);
        appended.adjoin(other,symbolic);
        ARIADNE_TEST_EQUALS(appended.locations().size(),2u);
        std::unique_ptr<HybridConstraintSet> clone(appended.clone());
        ARIADNE_TEST_EQUALS(clone->locations().size(),2u);

        HybridExactBox known(q,space,ExactBoxType({{0,1}}));
        HybridExactBox unknown(absent,space,ExactBoxType({{0,1}}));
        // The unknown location denotes an empty slice.
        ARIADNE_TEST_ASSERT(definitely(from_symbolic.separated(unknown)));
        static_cast<void>(from_symbolic.overlaps(known));
        static_cast<void>(from_symbolic.covers(known));
        static_cast<void>(from_symbolic.separated(known));
        static_cast<void>(from_symbolic.overlaps(unknown));
        static_cast<void>(from_symbolic.covers(unknown));

        std::ostringstream output;
        from_symbolic._write(output);
        ARIADNE_TEST_ASSERT(output.str().find("HybridConstraintSet")!=std::string::npos);
    }

    Void test_bounded_sets() {
        DiscreteLocation q(1), other(2), absent(3);
        RealVariable x("x");
        RealSpace space({x});
        RealExpressionBoundedConstraintSet symbolic({0<=x<=2});

        HybridBoundedConstraintSet empty;
        ARIADNE_TEST_ASSERT(empty.locations().empty());
        HybridBoundedConstraintSet from_symbolic(q,symbolic);
        HybridBoundedConstraintSet from_intervals(q,{0<=x<=2});
        HybridBoundedConstraintSet with_predicate(q,{0<=x<=2},{x>=0});
        RealVariablesBox raw({0<=x<=2});
        HybridBoundedConstraintSet from_box(q,raw);
        Set<DiscreteLocation> both={q,other};
        HybridBoundedConstraintSet multiple(both,symbolic);

        ARIADNE_TEST_EQUALS(multiple.locations().size(),2u);
        ARIADNE_TEST_ASSERT(from_symbolic.variables(q).contains(x));
        ARIADNE_TEST_ASSERT(from_intervals.variables(q).contains(x));
        ARIADNE_TEST_ASSERT(with_predicate.variables(q).contains(x));
        ARIADNE_TEST_ASSERT(from_box.variables(q).contains(x));
        ARIADNE_TEST_EQUALS(from_symbolic.euclidean_set(q,space).dimension(),1u);
        ARIADNE_TEST_ASSERT(from_symbolic.continuous_set(q).variables().contains(x));
        ARIADNE_TEST_ASSERT(are_same(from_symbolic.location(),q));
        ARIADNE_TEST_ASSERT(from_symbolic.bounding_box().has_location(q));

        HybridBoundedConstraintSet appended;
        appended.adjoin(q,symbolic);
        appended.adjoin(other,symbolic);
        ARIADNE_TEST_EQUALS(appended.locations().size(),2u);
        std::unique_ptr<HybridBoundedConstraintSet> clone(appended.clone());
        ARIADNE_TEST_EQUALS(clone->locations().size(),2u);

        HybridExactBox known(q,space,ExactBoxType({{0,1}}));
        HybridExactBox unknown(absent,space,ExactBoxType({{0,1}}));
        ARIADNE_TEST_ASSERT(definitely(from_symbolic.separated(unknown)));
        static_cast<void>(from_symbolic.overlaps(known));
        static_cast<void>(from_symbolic.covers(known));
        static_cast<void>(from_symbolic.separated(known));
        static_cast<void>(from_symbolic.overlaps(unknown));
        static_cast<void>(from_symbolic.covers(unknown));
        HybridExactBoxes boxes;
        boxes.insert(q,space,ExactBoxType({{0,2}}));
        static_cast<void>(from_symbolic.inside(boxes));

        std::ostringstream output;
        from_symbolic._write(output);
        ARIADNE_TEST_ASSERT(output.str().find("HybridBoundedConstraintSet")!=std::string::npos);
    }

    Void test_intersection_with_missing_constraint() {
        DiscreteLocation constrained(1), unconstrained(2);
        RealVariable x("x");
        RealSpace space({x});
        RealVariablesBox real_box({0<=x<=2});
        Set<DiscreteLocation> locations={constrained,unconstrained};
        HybridBoxesSet bounds(locations,real_box);

        List<ContinuousPredicate> predicates({x>=0});
        HybridConstraintSet restrictions(constrained,predicates);
        HybridBoundedConstraintSet result=intersection(bounds,restrictions);
        ARIADNE_TEST_EQUALS(result.locations().size(),2u);
        ARIADNE_TEST_ASSERT(result.locations().contains(constrained));
        ARIADNE_TEST_ASSERT(result.locations().contains(unconstrained));
        ARIADNE_TEST_EQUALS(result.euclidean_set(unconstrained,space).dimension(),1u);
        ARIADNE_TEST_ASSERT(result.bounding_box().has_location(unconstrained));
    }
};

Int main() {
    TestHybridConstraints().test();
    return ARIADNE_TEST_FAILURES;
}
