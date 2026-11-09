// Test hybrid paving operations against two discrete locations, with one
// deliberately empty location to exercise cross-location iteration.
#include <memory>
#include <sstream>
#include <string>

#include "ariadne-testing.hpp"
#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_bounded_constraint_set.hpp"

using namespace Ariadne;

class TestHybridPavingOperations : public Ariadne::Testing::ContextFixture {
public:
    using ContextFixture::ContextFixture;


    Void test_generic_shared_manager_locations() {
        DiscreteLocation first(1), second(2);
        RealVariable x("x");
        RealSpace space({x});
        MonolithicHybridSpace hspace;
        hspace.new_location(first,space);
        hspace.new_location(second,space);
        HybridGrid grid(hspace);
        auto manager=std::make_shared<RobddManager>();
        std::weak_ptr<RobddManager> weak=manager;
        HybridPaving paving(grid,manager);
        manager.reset();

        GridCell first_root(grid[first],0u,BinaryWord{});
        GridCell second_root(grid[second],0u,BinaryWord{});
        paving[first].adjoin(first_root.split(false));
        paving[second].adjoin(second_root.split(true));
        ARIADNE_TESTING_PROVE(!paving.is_empty());
        ARIADNE_TESTING_EXPECT_EQUAL(paving.size(),2u);
        ARIADNE_TESTING_PROVE(!weak.expired());

        // Cross-location Boolean operations reuse the same canonical manager.
        Paving joined=join(
            static_cast<PavingInterface const&>(paving[first]),
            static_cast<PavingInterface const&>(paving[second]));
        ARIADNE_TESTING_PROVE(joined.size()>0u);

        HybridPaving copy(paving);
        ARIADNE_TESTING_PROVE(subset(paving,copy));
        ARIADNE_TESTING_PROVE(subset(copy,paving));
        ARIADNE_TESTING_EXPECT_EQUAL(copy.size(),paving.size());
        copy.clear();
        ARIADNE_TESTING_PROVE(copy.is_empty());
        ARIADNE_TESTING_PROVE(!paving.is_empty());

        auto it=paving.begin();
        ARIADNE_TESTING_PROVE(it!=paving.end());
        ARIADNE_TESTING_PROVE((*it).location()==first);
        ++it;
        ARIADNE_TESTING_PROVE(it!=paving.end());
        ARIADNE_TESTING_PROVE((*it).location()==second);
        ++it;
        ARIADNE_TESTING_PROVE(it==paving.end());
    }

    Void test_paving_operations() {
        DiscreteLocation filled(1), vacant(2);
        RealVariable x("x");
        RealSpace space({x});
        MonolithicHybridSpace hspace;
        hspace.new_location(filled,space);
        hspace.new_location(vacant,space);
        HybridGrid grid(hspace);
        HybridBinaryTreePaving paving(grid);

        ARIADNE_TESTING_PROVE(paving.is_empty());
        ARIADNE_TESTING_EXPECT_EQUAL(paving.size(),0u);
        ARIADNE_TESTING_PROVE(paving.begin()==paving.end());
        paving[vacant];
        ARIADNE_TESTING_PROVE(paving.is_empty());
        HybridExactBoxes boxes;
        boxes.insert(filled,space,ExactBoxType({{0,1}}));
        paving.adjoin_outer_approximation(boxes,3u);
        ARIADNE_TESTING_PROVE(paving.nontrivial_location(filled));
        ARIADNE_TESTING_PROVE(!paving.is_empty());
        ARIADNE_TESTING_PROVE(paving.size()>0u);
        ARIADNE_TESTING_PROVE(paving.begin()!=paving.end());
        ARIADNE_TESTING_PROVE(paving.bounding_box().has_location(filled));

        // A nonempty paving can be exported as named-location boxes.
        // The list must retain the original spaces and all constituent cells.
        HybridListSet<ExactBoxType> exported=paving.boxes();
        ARIADNE_TESTING_EXPECT_EQUAL(exported.size(),paving.size());
        ARIADNE_TESTING_PROVE(exported.begin()!=exported.end());
        ARIADNE_TESTING_EXPECT_EQUAL((*exported.begin()).space().dimension(),1u);

        // The empty paving is contained even in an empty box collection.
        HybridBinaryTreePaving empty_paving(grid);
        HybridExactBoxes no_bounds;
        ARIADNE_TESTING_PROVE(definitely(empty_paving.inside(no_bounds)));
        // A nonempty location omitted from the containing collection is
        // not contained. This must not dereference an absent map entry.
        HybridExactBoxes wrong_location;
        wrong_location.insert(vacant,space,ExactBoxType({{-4,4}}));
        ARIADNE_TESTING_PROVE(!definitely(paving.inside(wrong_location)));
        HybridExactBoxes large_domain;
        large_domain.insert(filled,space,ExactBoxType({{-4,4}}));
        ARIADNE_TESTING_PROVE(definitely(paving.inside(large_domain)));

        // Both single-cell and list-cell insertion preserve the cell.
        HybridGridCell cell=*paving.begin();
        HybridBinaryTreePaving single_cell(grid);
        single_cell.adjoin(cell);
        ARIADNE_TESTING_PROVE(single_cell.size()>0u);
        ListSet<HybridGridCell> cell_list;
        cell_list.adjoin(cell);
        HybridBinaryTreePaving multiple_cells(grid);
        multiple_cells.adjoin(cell_list);
        ARIADNE_TESTING_EXPECT_EQUAL(single_cell.size(),multiple_cells.size());
        ARIADNE_TESTING_PROVE(subset(single_cell,multiple_cells));

        // The named grid, full hybrid space and stream API must agree.
        ARIADNE_TESTING_PROVE(paving.grid().has_location(filled));
        ARIADNE_TESTING_PROVE(paving.space().has_location(filled));
        std::ostringstream description;
        description << paving;
        ARIADNE_TESTING_PROVE(!description.str().empty());

        SizeType visited=0u;
        for(auto i=paving.begin();i!=paving.end();++i) { ++visited; }
        ARIADNE_TESTING_PROVE(visited>0u);

        std::unique_ptr<HybridBinaryTreePaving> clone(paving.clone());
        ARIADNE_TESTING_EXPECT_EQUAL(clone->size(),paving.size());
        HybridBinaryTreePaving copy(grid);
        copy.adjoin(paving);
        ARIADNE_TESTING_PROVE(!copy.is_empty());
        ARIADNE_TESTING_PROVE(subset(copy,paving));
        ARIADNE_TESTING_PROVE(subset(paving,copy));
        ARIADNE_TESTING_PROVE(intersect(copy,paving));

        copy.remove(paving);
        ARIADNE_TESTING_PROVE(copy.is_empty());
        ARIADNE_TESTING_PROVE(subset(copy,paving));
        ARIADNE_TESTING_PROVE(!intersect(copy,paving));

        copy.adjoin(paving);
        copy.restrict(paving);
        ARIADNE_TESTING_PROVE(!copy.is_empty());
        ARIADNE_TESTING_PROVE(subset(copy,paving));
        copy.mince(3u);
        copy.recombine();
        ARIADNE_TESTING_PROVE(!copy.is_empty());
        copy.restrict_to_extent(3u);
        ARIADNE_TESTING_PROVE(!copy.is_empty());
        copy.clear();
        ARIADNE_TESTING_PROVE(copy.is_empty());

        HybridBinaryTreePaving const& cpaving=paving;
        ARIADNE_TESTING_PROVE(!cpaving[filled].is_empty());
        std::ostringstream output;
        paving._write(output);
        ARIADNE_TESTING_PROVE(!output.str().empty());
    }

    Void test_empty_slice_relations() {
        DiscreteLocation loc(1),other(2); RealVariable x("x"); RealSpace space({x});
        MonolithicHybridSpace hspace; hspace.new_location(loc,space); hspace.new_location(other,space);
        HybridGrid grid(hspace);
        HybridBinaryTreePaving left(grid),empty(grid),disjoint(grid),superset(grid),other_slice(grid);
        HybridExactBoxes boxes; boxes.insert(loc,space,ExactBoxType({{0,1}}));
        left.adjoin_outer_approximation(boxes,2u);
        ARIADNE_TESTING_PROVE(!subset(left,empty));
        ARIADNE_TESTING_PROVE(!intersect(left,empty));
        empty[loc]; // Present but empty is equivalent to an absent slice.
        ARIADNE_TESTING_PROVE(!subset(left,empty));
        ARIADNE_TESTING_PROVE(!intersect(left,empty));
        HybridExactBoxes far; far.insert(loc,space,ExactBoxType({{4,5}}));
        disjoint.adjoin_outer_approximation(far,2u);
        ARIADNE_TESTING_PROVE(!subset(left,disjoint));
        ARIADNE_TESTING_PROVE(!intersect(left,disjoint));
        HybridExactBoxes large; large.insert(loc,space,ExactBoxType({{-1,2}}));
        superset.adjoin_outer_approximation(large,2u);
        ARIADNE_TESTING_PROVE(subset(left,superset));
        ARIADNE_TESTING_PROVE(intersect(left,superset));
        HybridExactBoxes elsewhere; elsewhere.insert(other,space,ExactBoxType({{0,1}}));
        other_slice.adjoin_outer_approximation(elsewhere,2u);
        HybridBinaryTreePaving restricted=left;
        restricted.restrict(other_slice);
        ARIADNE_TESTING_PROVE(restricted.is_empty());
        ARIADNE_TESTING_PROVE(!restricted.nontrivial_location(other));
        restricted=left; restricted.restrict(empty);
        ARIADNE_TESTING_PROVE(restricted.is_empty());
        restricted=left; restricted.restrict(disjoint);
        ARIADNE_TESTING_PROVE(restricted.is_empty());
        restricted=left; restricted.restrict(superset);
        ARIADNE_TESTING_PROVE(subset(left,restricted));
        ARIADNE_TESTING_PROVE(subset(restricted,left));
        restricted.restrict(restricted);
        ARIADNE_TESTING_PROVE(subset(left,restricted));
        HybridBinaryTreePaving nothing(grid); nothing.restrict(left);
        ARIADNE_TESTING_PROVE(nothing.is_empty());
    }
    Void test_contracts_and_iterator_comparisons() {
        DiscreteLocation first(1),empty(2),last(3),missing(4);
        RealVariable x("x"),y("y"); RealSpace space({x});
        MonolithicHybridSpace hspace;
        hspace.new_location(first,space); hspace.new_location(empty,space); hspace.new_location(last,space);
        HybridBinaryTreePaving paving{HybridGrid(hspace)};
        HybridExactBoxes boxes;
        boxes.insert(first,space,ExactBoxType({{0,1}}));
        boxes.insert(last,space,ExactBoxType({{0,1}}));
        paving.adjoin_outer_approximation(boxes,2u); paving[empty]; paving.mince(2u);
        const HybridBinaryTreePaving& immutable=paving;
        ARIADNE_TESTING_EXPECT_EXCEPTION(immutable[missing]);
        ARIADNE_TESTING_EXPECT_EXCEPTION(immutable.euclidean_set(first,RealSpace({y})));
        HybridExactBoxes wrong_space; wrong_space.insert(first,RealSpace({y}),ExactBoxType({{0,1}}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(paving.adjoin_inner_approximation(wrong_space,1u));
        ARIADNE_TESTING_EXPECT_EXCEPTION(paving.adjoin_outer_approximation(wrong_space,1u));
        auto begin=paving.begin(); auto shifted=begin;
        ARIADNE_TESTING_PROVE(begin==shifted);
        ++shifted;
        ARIADNE_TESTING_PROVE((*begin).location()==(*shifted).location());
        ARIADNE_TESTING_PROVE(begin!=shifted);
        while(shifted!=paving.end() && (*shifted).location()==first) ++shifted;
        ARIADNE_TESTING_PROVE(shifted!=paving.end());
        ARIADNE_TESTING_PROVE((*shifted).location()==last);
        ARIADNE_TESTING_PROVE(begin!=shifted);
        while(shifted!=paving.end()) ++shifted;
        ARIADNE_TESTING_PROVE(shifted==paving.end());
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
        HybridBinaryTreePaving inner(grid);
        inner.adjoin_inner_approximation(boxes,2u);
        ARIADNE_TESTING_PROVE(inner.has_location(loc));

        HybridBoundedConstraintSet bounded(loc,{0<=x<=2});
        HybridBinaryTreePaving outer(grid);
        outer.adjoin_outer_approximation(bounded,3u);
        ARIADNE_TESTING_PROVE(outer.nontrivial_location(loc));
        ARIADNE_TESTING_PROVE(outer.size()>0u);
        HybridBinaryTreePaving via_interface(grid);
        EffectiveHybridSetInterface const& effective=bounded;
        via_interface.adjoin_inner_approximation(effective,2u);
        ARIADNE_TESTING_PROVE(via_interface.has_location(loc));

        // The free function delegates to the same inner approximation.
        HybridBinaryTreePaving constructed=inner_approximation(effective,grid,2u);
        ARIADNE_TESTING_PROVE(constructed.has_location(loc));

        // Lower approximation uses the overt view and must populate
        // the location even when no full cell can be proved inside.
        const EffectiveHybridOvertSetInterface& overt=bounded;
        HybridBinaryTreePaving lower(grid);
        lower.adjoin_lower_approximation(overt,2u,2u);
        ARIADNE_TESTING_PROVE(lower.nontrivial_location(loc));
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_paving_operations");
    ARIADNE_TESTING_ADD_TEST(TestHybridPavingOperations, test_generic_shared_manager_locations);
    ARIADNE_TESTING_ADD_TEST(TestHybridPavingOperations, test_paving_operations);
    ARIADNE_TESTING_ADD_TEST(TestHybridPavingOperations, test_inner_and_outer_approximation);
    ARIADNE_TESTING_ADD_TEST(TestHybridPavingOperations, test_empty_slice_relations);
    ARIADNE_TESTING_ADD_TEST(TestHybridPavingOperations, test_contracts_and_iterator_comparisons);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
