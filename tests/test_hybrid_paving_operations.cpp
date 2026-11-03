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
        ARIADNE_TEST_CALL(test_empty_slice_relations());
        ARIADNE_TEST_CALL(test_contracts_and_iterator_comparisons());
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

        // A nonempty paving can be exported as named-location boxes.
        // The list must retain the original spaces and all constituent cells.
        HybridListSet<ExactBoxType> exported=paving.boxes();
        ARIADNE_TEST_EQUALS(exported.size(),paving.size());
        ARIADNE_TEST_ASSERT(exported.begin()!=exported.end());
        ARIADNE_TEST_EQUALS((*exported.begin()).space().dimension(),1u);

        // The empty paving is contained even in an empty box collection.
        HybridGridTreePaving empty_paving(grid);
        HybridExactBoxes no_bounds;
        ARIADNE_TEST_ASSERT(definitely(empty_paving.inside(no_bounds)));
        // A nonempty location omitted from the containing collection is
        // not contained. This must not dereference an absent map entry.
        HybridExactBoxes wrong_location;
        wrong_location.insert(vacant,space,ExactBoxType({{-4,4}}));
        ARIADNE_TEST_ASSERT(!definitely(paving.inside(wrong_location)));
        HybridExactBoxes large_domain;
        large_domain.insert(filled,space,ExactBoxType({{-4,4}}));
        ARIADNE_TEST_ASSERT(definitely(paving.inside(large_domain)));

        // Both single-cell and list-cell insertion preserve the cell.
        HybridGridCell cell=*paving.begin();
        HybridGridTreePaving single_cell(grid);
        single_cell.adjoin(cell);
        ARIADNE_TEST_ASSERT(single_cell.size()>0u);
        ListSet<HybridGridCell> cell_list;
        cell_list.adjoin(cell);
        HybridGridTreePaving multiple_cells(grid);
        multiple_cells.adjoin(cell_list);
        ARIADNE_TEST_EQUALS(single_cell.size(),multiple_cells.size());
        ARIADNE_TEST_ASSERT(subset(single_cell,multiple_cells));

        // The named grid, full hybrid space and stream API must agree.
        ARIADNE_TEST_ASSERT(paving.grid().has_location(filled));
        ARIADNE_TEST_ASSERT(paving.space().has_location(filled));
        std::ostringstream description;
        description << paving;
        ARIADNE_TEST_ASSERT(!description.str().empty());

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

    Void test_empty_slice_relations() {
        DiscreteLocation loc(1),other(2); RealVariable x("x"); RealSpace space({x});
        MonolithicHybridSpace hspace; hspace.new_location(loc,space); hspace.new_location(other,space);
        HybridGrid grid(hspace);
        HybridGridTreePaving left(grid),empty(grid),disjoint(grid),superset(grid),other_slice(grid);
        HybridExactBoxes boxes; boxes.insert(loc,space,ExactBoxType({{0,1}}));
        left.adjoin_outer_approximation(boxes,2u);
        ARIADNE_TEST_ASSERT(!subset(left,empty));
        ARIADNE_TEST_ASSERT(!intersect(left,empty));
        empty[loc]; // Present but empty is equivalent to an absent slice.
        ARIADNE_TEST_ASSERT(!subset(left,empty));
        ARIADNE_TEST_ASSERT(!intersect(left,empty));
        HybridExactBoxes far; far.insert(loc,space,ExactBoxType({{4,5}}));
        disjoint.adjoin_outer_approximation(far,2u);
        ARIADNE_TEST_ASSERT(!subset(left,disjoint));
        ARIADNE_TEST_ASSERT(!intersect(left,disjoint));
        HybridExactBoxes large; large.insert(loc,space,ExactBoxType({{-1,2}}));
        superset.adjoin_outer_approximation(large,2u);
        ARIADNE_TEST_ASSERT(subset(left,superset));
        ARIADNE_TEST_ASSERT(intersect(left,superset));
        HybridExactBoxes elsewhere; elsewhere.insert(other,space,ExactBoxType({{0,1}}));
        other_slice.adjoin_outer_approximation(elsewhere,2u);
        HybridGridTreePaving restricted=left;
        restricted.restrict(other_slice);
        ARIADNE_TEST_ASSERT(restricted.is_empty());
        ARIADNE_TEST_ASSERT(!restricted.nontrivial_location(other));
        restricted=left; restricted.restrict(empty);
        ARIADNE_TEST_ASSERT(restricted.is_empty());
        restricted=left; restricted.restrict(disjoint);
        ARIADNE_TEST_ASSERT(restricted.is_empty());
        restricted=left; restricted.restrict(superset);
        ARIADNE_TEST_ASSERT(subset(left,restricted));
        ARIADNE_TEST_ASSERT(subset(restricted,left));
        restricted.restrict(restricted);
        ARIADNE_TEST_ASSERT(subset(left,restricted));
        HybridGridTreePaving nothing(grid); nothing.restrict(left);
        ARIADNE_TEST_ASSERT(nothing.is_empty());
    }
    Void test_contracts_and_iterator_comparisons() {
        DiscreteLocation first(1),empty(2),last(3),missing(4);
        RealVariable x("x"),y("y"); RealSpace space({x});
        MonolithicHybridSpace hspace;
        hspace.new_location(first,space); hspace.new_location(empty,space); hspace.new_location(last,space);
        HybridGridTreePaving paving{HybridGrid(hspace)};
        HybridExactBoxes boxes;
        boxes.insert(first,space,ExactBoxType({{0,1}}));
        boxes.insert(last,space,ExactBoxType({{0,1}}));
        paving.adjoin_outer_approximation(boxes,2u); paving[empty]; paving.mince(2u);
        const HybridGridTreePaving& immutable=paving;
        ARIADNE_TEST_FAIL(immutable[missing]);
        ARIADNE_TEST_FAIL(immutable.euclidean_set(first,RealSpace({y})));
        HybridExactBoxes wrong_space; wrong_space.insert(first,RealSpace({y}),ExactBoxType({{0,1}}));
        ARIADNE_TEST_FAIL(paving.adjoin_inner_approximation(wrong_space,1u));
        ARIADNE_TEST_FAIL(paving.adjoin_outer_approximation(wrong_space,1u));
        auto begin=paving.begin(); auto shifted=begin;
        ARIADNE_TEST_ASSERT(begin==shifted);
        ++shifted;
        ARIADNE_TEST_ASSERT((*begin).location()==(*shifted).location());
        ARIADNE_TEST_ASSERT(begin!=shifted);
        while(shifted!=paving.end() && (*shifted).location()==first) ++shifted;
        ARIADNE_TEST_ASSERT(shifted!=paving.end());
        ARIADNE_TEST_ASSERT((*shifted).location()==last);
        ARIADNE_TEST_ASSERT(begin!=shifted);
        while(shifted!=paving.end()) ++shifted;
        ARIADNE_TEST_ASSERT(shifted==paving.end());
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

        // The free function delegates to the same inner approximation.
        HybridGridTreePaving constructed=inner_approximation(effective,grid,2u);
        ARIADNE_TEST_ASSERT(constructed.has_location(loc));

        // Lower approximation uses the overt view and must populate
        // the location even when no full cell can be proved inside.
        const EffectiveHybridOvertSetInterface& overt=bounded;
        HybridGridTreePaving lower(grid);
        lower.adjoin_lower_approximation(overt,2u,2u);
        ARIADNE_TEST_ASSERT(lower.nontrivial_location(loc));
    }
};

Int main() {
    TestHybridPavingOperations().test();
    return ARIADNE_TEST_FAILURES;
}
