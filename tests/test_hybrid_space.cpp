// Behavioral coverage for hybrid spaces, location lookup and grids.
#include <memory>
#include <sstream>
#include <string>
#include "macro/testing.hpp"
#include "hybrid/hybrid_space.hpp"
#include "hybrid/hybrid_grid.hpp"

using namespace Ariadne;

class TestHybridSpace {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_location_spaces());
        ARIADNE_TEST_CALL(test_grid_and_scalings());
    }

    Void test_location_spaces() {
        DiscreteLocation first(1), second(2), missing(3);
        RealVariable x("x"), y("y");
        RealSpace one({x}), two({x,y});

        MonolithicHybridSpace m;
        m.new_location(first,one);
        m.new_location(second,two);
        ARIADNE_TEST_ASSERT(m.has_location(first));
        ARIADNE_TEST_ASSERT(m.has_location(second));
        ARIADNE_TEST_ASSERT(!m.has_location(missing));
        ARIADNE_TEST_EQUALS(m[first].dimension(),1u);
        ARIADNE_TEST_EQUALS(m[second].dimension(),2u);
        ARIADNE_TEST_FAIL(static_cast<void>(m[missing]));
        ARIADNE_TEST_EQUALS(m.locations().size(),2u);
        ARIADNE_TEST_ASSERT(m.begin()!=m.end());

        MonolithicHybridSpace same;
        same.new_location(first,one);
        same.new_location(second,two);
        MonolithicHybridSpace altered;
        altered.new_location(first,two);
        altered.new_location(second,two);
        MonolithicHybridSpace partial;
        partial.new_location(first,one);
        ARIADNE_TEST_ASSERT(definitely(m==same));
        ARIADNE_TEST_ASSERT(!definitely(m==altered));
        ARIADNE_TEST_ASSERT(!definitely(m==partial));

        HybridSpace polymorphic(m);
        ARIADNE_TEST_ASSERT(polymorphic.has_location(second));
        ARIADNE_TEST_EQUALS(polymorphic[second].dimension(),2u);
        HybridSpace cloned(new MonolithicHybridSpace(m));
        ARIADNE_TEST_ASSERT(cloned.has_location(first));
        ARIADNE_TEST_ASSERT(definitely(polymorphic==cloned));

        std::ostringstream output;
        output << polymorphic;
        ARIADNE_TEST_ASSERT(output.str().find("HybridSpace")!=std::string::npos);
    }

    Void test_grid_and_scalings() {
        DiscreteLocation q(1), missing(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x,y});
        MonolithicHybridSpace m;
        m.new_location(q,spc);
        HybridSpace hspc(m);

        HybridGrid unit(hspc);
        ARIADNE_TEST_ASSERT(unit.has_location(q));
        ARIADNE_TEST_ASSERT(!unit.has_location(missing));
        ARIADNE_TEST_EQUALS(unit.space(q).dimension(),2u);
        ARIADNE_TEST_EQUALS(unit[q].dimension(),2u);
        ARIADNE_TEST_EQUALS(unit.grid(q).dimension(),2u);

        SimpleHybridScalings policy;
        HybridScalings scalings(policy);
        HybridGrid scaled(hspc,scalings);
        ARIADNE_TEST_EQUALS(scaled.grid(q).dimension(),2u);
        ARIADNE_TEST_EQUALS(scaled.space(q).dimension(),2u);
        std::unique_ptr<HybridGrid> cloned_grid(scaled.clone());
        ARIADNE_TEST_EQUALS(cloned_grid->grid(q).dimension(),2u);
        std::ostringstream output;
        output << scaled;
        ARIADNE_TEST_ASSERT(output.str().find("HybridGrid")!=std::string::npos);
    }
};

Int main() {
    TestHybridSpace().test();
    return ARIADNE_TEST_FAILURES;
}
