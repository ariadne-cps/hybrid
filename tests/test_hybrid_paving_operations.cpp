// Test hybrid paving operations against two discrete locations, with one
// deliberately empty location to exercise cross-location iteration.
#include <memory>
#include <sstream>
#include <string>

#include "macro/testing.hpp"
#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_bounded_constraint_set.hpp"

using namespace Ariadne;

class TestHybridPavingOperations {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_paving_operations());
        ARIADNE_TEST_CALL(test_inner_and_outer_approximation());
    }

    Void test_paving_operations() {
        DiscreteLocation filled(1), vacant(2);
        RealVariable x("x");
        RealSpace space({x});
        MonolithicHybridSpace hspace;
        hspace.new_location(filled,space);
        hspace.new_location(vacant,space);
        HybridGrid grid(hspace);
        HybridGridTreePaving paving(grid);

        ARIADNE_TEST_ASSERT(paving.is_empty());
        ARIADNE_TEST_EQUALS(paving.size(),0u);
        ARIADNE_TEST_ASSERT(paving.begin()==paving.end());
        paving[vacant];
        ARIADNE_TEST_ASSERT(paving.is_empty());
        HybridExactBoxes boxes;
        boxes.insert(filled,space,ExactBoxType({{0,1}}));
        paving.adjoin_outer_approximation(boxes,3u);
        ARIADNE_TEST_ASSERT(paving.nontrivial_location(filled));
        ARIADNE_TEST_ASSERT(!paving.is_empty());
        ARIADNE_TEST_ASSERT(paving.size()>0u);
        ARIADNE_TEST_ASSERT(paving.begin()!=paving.end());
        ARIADNE_TEST_ASSERT(paving.bounding_box().has_location(filled));

        SizeType visited=0u;
        for(auto i=paving.begin();i!=paving.end();++i) { ++visited; }
        ARIADNE_TEST_ASSERT(visited>0u);

        std::unique_ptr<HybridGridTreePaving> clone(paving.clone());
        ARIADNE_TEST_EQUALS(clone->size(),paving.size());
        HybridGridTreePaving copy(grid);
        copy.adjoin(paving);
        ARIADNE_TEST_ASSERT(!copy.is_empty());
        ARIADNE_TEST_ASSERT(subset(copy,paving));
        ARIADNE_TEST_ASSERT(subset(paving,copy));
        ARIADNE_TEST_ASSERT(intersect(copy,paving));

        copy.remove(paving);
        ARIADNE_TEST_ASSERT(copy.is_empty());
        ARIADNE_TEST_ASSERT(subset(copy,paving));
        ARIADNE_TEST_ASSERT(!intersect(copy,paving));

        copy.adjoin(paving);
        copy.restrict(paving);
        ARIADNE_TEST_ASSERT(!copy.is_empty());
        ARIADNE_TEST_ASSERT(subset(copy,paving));
        copy.mince(3u);
        copy.recombine();
        ARIADNE_TEST_ASSERT(!copy.is_empty());
        copy.restrict_to_extent(3u);
        ARIADNE_TEST_ASSERT(!copy.is_empty());
        copy.clear();
        ARIADNE_TEST_ASSERT(copy.is_empty());

        HybridGridTreePaving const& cpaving=paving;
        ARIADNE_TEST_ASSERT(!cpaving[filled].is_empty());
        std::ostringstream output;
        paving._write(output);
        ARIADNE_TEST_ASSERT(!output.str().empty());
    }

    Void test_inner_and_outer_approximation() {
        DiscreteLocation loc(1);
        RealVariable x("x");
        RealSpace spc({x});
        MonolithicHybridSpace hspace;
        hspace.new_location(loc,spc);
        HybridGrid grid(hspace);

        HybridExactBoxes boxes;
        boxes.insert(loc,spc,ExactBoxType({{0,2}}));
        HybridGridTreePaving inner(grid);
        inner.adjoin_inner_approximation(boxes,2u);
        ARIADNE_TEST_ASSERT(inner.has_location(loc));

        HybridBoundedConstraintSet bounded(loc,{0<=x<=2});
        HybridGridTreePaving outer(grid);
        outer.adjoin_outer_approximation(bounded,3u);
        ARIADNE_TEST_ASSERT(outer.nontrivial_location(loc));
        ARIADNE_TEST_ASSERT(outer.size()>0u);
        HybridGridTreePaving via_interface(grid);
        EffectiveHybridSetInterface const& effective=bounded;
        via_interface.adjoin_inner_approximation(effective,2u);
        ARIADNE_TEST_ASSERT(via_interface.has_location(loc));
    }
};

Int main() {
    TestHybridPavingOperations().test();
    return ARIADNE_TEST_FAILURES;
}
