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
        ARIADNE_TEST_ASSERT(!definitely(hybrid_box.is_empty()));

        std::unique_ptr<HybridBoxSet> clone(hybrid_box.clone());
        ARIADNE_TEST_EQUALS(clone->space().dimension(),2u);
        ARIADNE_TEST_ASSERT(clone->locations().contains(location));
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
