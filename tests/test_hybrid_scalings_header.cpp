// Regression: the public header must compile without an umbrella header.
#include "hybrid/hybrid_scalings.hpp"

#include <memory>
#include <sstream>
#include <string>
#include "macro/testing.hpp"

using namespace Ariadne;

class TestHybridScalings {
public:
    Void test() {
        ARIADNE_TEST_CALL(test_named_and_default_scalings());
        ARIADNE_TEST_CALL(test_initializer_scalings());
        ARIADNE_TEST_CALL(test_setter_contract_and_interface());
    }
    Void test_named_and_default_scalings() {
        const DiscreteLocation first(1), second(2);
        const RealVariable x("x"), y("y");
        const Map<Identifier,ExactDouble> values{{x.name(),2.0_x}};
        SimpleHybridScalings custom(3.0_x,values);
        ARIADNE_TEST_EQUALS(custom.scaling(first,x),2.0_x);
        ARIADNE_TEST_EQUALS(custom.scaling(second,y),3.0_x);
        HybridScalings mapped(values);
        ARIADNE_TEST_EQUALS(mapped.scaling(first,x),2.0_x);
        ARIADNE_TEST_EQUALS(mapped.scaling(first,y),1.0_x);
        std::unique_ptr<SimpleHybridScalings> copy(custom.clone());
        copy->set_scaling(x,ApproximateDouble(4.0));
        ARIADNE_TEST_EQUALS(copy->scaling(first,x),4.0_x);
        ARIADNE_TEST_EQUALS(custom.scaling(first,x),2.0_x);
        std::ostringstream output;
        output<<static_cast<const HybridScalingsInterface&>(custom);
        ARIADNE_TEST_ASSERT(output.str().find("HybridScalings")!=std::string::npos);
        ARIADNE_TEST_ASSERT(output.str().find("x")!=std::string::npos);
    }
    Void test_initializer_scalings() {
        const DiscreteLocation loc(1);
        const RealVariable x("x"), y("y");
        const InitializerList<Pair<RealVariable,ApproximateDouble>> values{{x,ApproximateDouble(2.0)}};
        SimpleHybridScalings direct(values);
        HybridScalings wrapped(values);
        ARIADNE_TEST_EQUALS(direct.scaling(loc,x),2.0_x);
        ARIADNE_TEST_EQUALS(direct.scaling(loc,y),1.0_x);
        ARIADNE_TEST_EQUALS(wrapped.scaling(loc,y),1.0_x);
        SimpleHybridScalings empty(InitializerList<Pair<RealVariable,ApproximateDouble>>{});
        ARIADNE_TEST_EQUALS(empty.scaling(loc,x),1.0_x);
        auto pair=x|ApproximateDouble(4.0);
        ARIADNE_TEST_EQUALS(pair.first.name(),x.name());
        ARIADNE_TEST_EQUALS(pair.second,4.0_x);
    }
    Void test_setter_contract_and_interface() {
        const DiscreteLocation loc(1);
        const RealVariable x("x");
        SimpleHybridScalings policy;
        policy.set_scaling(x,ApproximateDouble(2.0));
        ARIADNE_TEST_FAIL(policy.set_scaling(x,ApproximateDouble(0.0)));
        ARIADNE_TEST_FAIL(policy.set_scaling(x,ApproximateDouble(-1.0)));
        ARIADNE_TEST_EQUALS(policy.scaling(loc,x),2.0_x);
        HybridScalings wrapped(policy);
        HybridScalingsInterface& mutable_view=wrapped;
        ARIADNE_TEST_EQUALS(mutable_view.scaling(loc,x),2.0_x);
        const HybridScalings& const_wrapper=wrapped;
        const HybridScalingsInterface& immutable_view=const_wrapper;
        ARIADNE_TEST_EQUALS(immutable_view.scaling(loc,x),2.0_x);
    }
};

int main() {
    TestHybridScalings().test();
    return ARIADNE_TEST_FAILURES;
}
