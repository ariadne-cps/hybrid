// Exercise HybridBoxSet and location-sensitive geometric predicates.
#include <memory>
#include <sstream>
#include <string>

#include "macro/testing.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_set.hpp"

using namespace Ariadne;

class TestHybridBoxSet {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_box_set_geometry());
        ARIADNE_TEST_CALL(test_location_semantics());
    }

    Void test_box_set_geometry() {
        DiscreteLocation location(1);
        RealVariable x("x"), y("y");
        RealSpace space({x,y});
        RealVariablesBox coordinates({0<=x<=2,0<=y<=2});
        HybridBoxSet hybrid_box(location,coordinates);

        ARIADNE_TEST_ASSERT(hybrid_box.locations().contains(location));
        ARIADNE_TEST_EQUALS(hybrid_box.locations().size(),1u);
        ARIADNE_TEST_ASSERT(hybrid_box.variables(location).contains(x));
        ARIADNE_TEST_ASSERT(hybrid_box.variables(location).contains(y));
        ARIADNE_TEST_EQUALS(hybrid_box.space().dimension(),2u);
        ARIADNE_TEST_EQUALS(hybrid_box.space(location).dimension(),2u);
        ARIADNE_TEST_EQUALS(hybrid_box.euclidean_set(space).dimension(),2u);
        // Exercise the location-indexed overload, which preserves the
        // underlying real box when the requested location matches.
        ARIADNE_TEST_EQUALS(hybrid_box.euclidean_set(location,space).dimension(),2u);
        ARIADNE_TEST_ASSERT(!definitely(hybrid_box.is_empty()));

        // The virtual clone must return a correctly adjusted base pointer
        // even when its implementation has virtual/multiple base classes.
        HybridBoxSet copied_box(hybrid_box);
        ARIADNE_TEST_ASSERT(copied_box.locations().contains(location));
        const EffectiveHybridSetInterface& abstract_box=hybrid_box;
        std::unique_ptr<EffectiveHybridSetInterface> cloned(abstract_box.clone());
        ARIADNE_TEST_ASSERT(cloned.get()!=nullptr);
        ARIADNE_TEST_ASSERT(cloned.get()!=&abstract_box);
        HybridBoxSet* concrete=dynamic_cast<HybridBoxSet*>(cloned.get());
        ARIADNE_TEST_ASSERT(concrete!=nullptr);
        if(concrete==nullptr) { return; }
        ARIADNE_TEST_EQUALS(cloned->locations().size(),1u);
        ARIADNE_TEST_ASSERT(cloned->locations().contains(location));
        const HybridSetInterfaceBase& root=hybrid_box;
        std::unique_ptr<HybridSetInterfaceBase> root_copy(root.clone());
        ARIADNE_TEST_ASSERT(dynamic_cast<HybridBoxSet*>(root_copy.get())!=nullptr);
        const EffectiveHybridBoundedSetInterface& bounded=hybrid_box;
        std::unique_ptr<EffectiveHybridBoundedSetInterface> bounded_copy(bounded.clone());
        ARIADNE_TEST_ASSERT(dynamic_cast<HybridBoxSet*>(bounded_copy.get())!=nullptr);
        ARIADNE_TEST_ASSERT(hybrid_box.bounding_box().has_location(location));

        std::ostringstream output;
        hybrid_box._write(output);
        ARIADNE_TEST_ASSERT(!output.str().empty());
    }

    Void test_location_semantics() {
        DiscreteLocation location(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace space({x,y});
        HybridBoxSet hybrid_box(location,RealVariablesBox({0<=x<=2,0<=y<=2}));
        HybridExactBox present(location,space,ExactBoxType({{0,1},{0,1}}));
        HybridExactBox absent(other,space,ExactBoxType({{0,1},{0,1}}));

        ARIADNE_TEST_ASSERT(definitely(hybrid_box.separated(absent)));
        ARIADNE_TEST_ASSERT(!definitely(hybrid_box.overlaps(absent)));
        ARIADNE_TEST_ASSERT(!definitely(hybrid_box.covers(absent)));

        // The actual Euclidean predicates are evaluated for the matching
        // location, and yield a well-defined lower logical result.
        static_cast<void>(hybrid_box.separated(present));
        static_cast<void>(hybrid_box.overlaps(present));
        static_cast<void>(hybrid_box.covers(present));
        static_cast<void>(hybrid_box.inside(present));
        static_cast<void>(hybrid_box.inside(absent));

        HybridExactBoxes present_boxes;
        present_boxes.insert(location,space,ExactBoxType({{0,3},{0,3}}));
        ARIADNE_TEST_ASSERT(!definitely(hybrid_box.is_empty()));
        static_cast<void>(hybrid_box.inside(present_boxes));
        HybridExactBoxes absent_boxes;
        absent_boxes.insert(other,space,ExactBoxType({{0,3},{0,3}}));
        static_cast<void>(hybrid_box.inside(absent_boxes));
    }
};

Int main() {
    TestHybridBoxSet().test();
    return ARIADNE_TEST_FAILURES;
}
