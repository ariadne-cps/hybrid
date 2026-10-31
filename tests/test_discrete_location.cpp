// Targeted behavioral coverage for hybrid discrete locations.
// Tests public contracts rather than constructing invalid internal states.
#include "macro/testing.hpp"
#include "hybrid/discrete_location.hpp"
#include "symbolic/valuation.hpp"

using namespace Ariadne;

class TestDiscreteLocation {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_construction_and_combination());
        ARIADNE_TEST_CALL(test_equality_and_order());
        ARIADNE_TEST_CALL(test_distinguishability_and_restriction());
    }

    Void test_construction_and_combination() {
        StringVariable a("a"), b("b");
        String zero("zero"), one("one");

        DiscreteLocation empty;
        DiscreteLocation left(a,zero);
        DiscreteLocation right(b,one);
        DiscreteLocation combined=join(left,right);

        ARIADNE_TEST_ASSERT(are_same(empty,DiscreteLocation()));
        ARIADNE_TEST_ASSERT(is_restriction(left,combined));
        ARIADNE_TEST_ASSERT(is_restriction(right,combined));
        ARIADNE_TEST_ASSERT(!are_same(left,combined));

        DiscreteLocation appended(left);
        appended.adjoin(right);
        ARIADNE_TEST_ASSERT(are_same(appended,combined));

        Map<StringVariable,String> variable_map;
        variable_map.insert(a,zero);
        DiscreteLocation from_variable_map(variable_map);
        ARIADNE_TEST_ASSERT(are_same(from_variable_map,left));

        Map<Identifier,String> identifier_map;
        identifier_map.insert(Identifier("a"),zero);
        DiscreteLocation from_identifier_map(identifier_map);
        ARIADNE_TEST_ASSERT(are_same(from_identifier_map,left));

        StringValuation valuation(variable_map);
        DiscreteLocation from_valuation(valuation);
        ARIADNE_TEST_ASSERT(are_same(from_valuation,left));

        DiscreteLocation from_initializer({a|zero,b|one});
        ARIADNE_TEST_ASSERT(are_same(from_initializer,combined));

        // The valid call retains every supplied key. The current implementation
        // requires the argument set to include the location's variable set.
        Set<Identifier> keys={Identifier("a"),Identifier("b")};
        DiscreteLocation retained=restrict(combined,keys);
        ARIADNE_TEST_ASSERT(are_same(retained,combined));
    }

    Void test_equality_and_order() {
        StringVariable a("a"), b("b");
        DiscreteLocation a0(a,String("0"));
        DiscreteLocation a1(a,String("1"));
        DiscreteLocation b0(b,String("0"));
        DiscreteLocation a0b0({a|String("0"),b|String("0")});
        DiscreteLocation a0b1({a|String("0"),b|String("1")});
        DiscreteLocation a1b0({a|String("1"),b|String("0")});

        ARIADNE_TEST_ASSERT(a0==a0);
        ARIADNE_TEST_ASSERT(a0!=a1);
        ARIADNE_TEST_ASSERT(a0<a1);
        ARIADNE_TEST_ASSERT(!(a1<a0));
        ARIADNE_TEST_ASSERT(a0<b0);
        ARIADNE_TEST_ASSERT(!(b0<a0));
        ARIADNE_TEST_ASSERT(a0b0<a0b1);
        ARIADNE_TEST_ASSERT(!(a0b1<a0b0));
        ARIADNE_TEST_ASSERT(a0b0<a1b0);
        ARIADNE_TEST_ASSERT(a0<a0b0);
        ARIADNE_TEST_ASSERT(!(a0b0<a0));

        // A common variable with different values is ordinary inequality.
        ARIADNE_TEST_ASSERT(!(a0b0==a0b1));

        // Distinct or partially specified valuations with no value conflict
        // cannot be compared for equality: the API explicitly throws.
        ARIADNE_TEST_FAIL(a0==b0);
        ARIADNE_TEST_FAIL(a0==a0b0);
        ARIADNE_TEST_FAIL(a0b0==a0);
    }

    Void test_distinguishability_and_restriction() {
        StringVariable a("a"), b("b"), c("c");
        DiscreteLocation empty;
        DiscreteLocation a0(a,String("0"));
        DiscreteLocation a1(a,String("1"));
        DiscreteLocation b0(b,String("0"));
        DiscreteLocation c0(c,String("0"));
        DiscreteLocation full({a|String("0"),b|String("0")});

        ARIADNE_TEST_ASSERT(are_distinguishable(a0,a1));
        ARIADNE_TEST_ASSERT(!are_distinguishable(empty,a0));
        ARIADNE_TEST_ASSERT(!are_distinguishable(a0,empty));
        ARIADNE_TEST_ASSERT(!are_distinguishable(a0,b0));
        ARIADNE_TEST_ASSERT(!are_distinguishable(a0,full));
        ARIADNE_TEST_ASSERT(!are_same(a0,a1));
        ARIADNE_TEST_ASSERT(are_same(a0,a0));

        ARIADNE_TEST_ASSERT(is_restriction(empty,full));
        ARIADNE_TEST_ASSERT(is_restriction(a0,full));
        ARIADNE_TEST_ASSERT(is_restriction(full,full));
        ARIADNE_TEST_ASSERT(!is_restriction(a1,full));
        ARIADNE_TEST_ASSERT(!is_restriction(c0,full));
        ARIADNE_TEST_ASSERT(!is_restriction(full,a0));
    }
};

Int main() {
    TestDiscreteLocation().test();
    return ARIADNE_TEST_FAILURES;
}
