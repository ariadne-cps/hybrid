/***************************************************************************
 *            tests/test_hybrid_scalings_header.cpp
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

/** @file tests/test_hybrid_scalings_header.cpp
 *  @brief Exercise test hybrid scalings header behavior.
 */

// Regression: the public header must compile without an umbrella header.
#include "hybrid/hybrid_scalings.hpp"

#include <memory>
#include <sstream>
#include <string>
#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestHybridScalings : public Ariadne::Testing::ContextFixture {
public:
    using ContextFixture::ContextFixture;

    Void test_named_and_default_scalings() {
        const DiscreteLocation first(1), second(2);
        const RealVariable x("x"), y("y");
        const Map<Identifier,ExactDouble> values{{x.name(),2.0_x}};
        SimpleHybridScalings custom(3.0_x,values);
        ARIADNE_TESTING_EXPECT_EQUAL(custom.scaling(first,x),2.0_x);
        ARIADNE_TESTING_EXPECT_EQUAL(custom.scaling(second,y),3.0_x);
        HybridScalings mapped(values);
        ARIADNE_TESTING_EXPECT_EQUAL(mapped.scaling(first,x),2.0_x);
        ARIADNE_TESTING_EXPECT_EQUAL(mapped.scaling(first,y),1.0_x);
        std::unique_ptr<SimpleHybridScalings> copy(custom.clone());
        copy->set_scaling(x,ApproximateDouble(4.0));
        ARIADNE_TESTING_EXPECT_EQUAL(copy->scaling(first,x),4.0_x);
        ARIADNE_TESTING_EXPECT_EQUAL(custom.scaling(first,x),2.0_x);
        std::ostringstream output;
        output<<static_cast<const HybridScalingsInterface&>(custom);
        ARIADNE_TESTING_PROVE(output.str().find("HybridScalings")!=std::string::npos);
        ARIADNE_TESTING_PROVE(output.str().find("x")!=std::string::npos);
    }
    Void test_initializer_scalings() {
        const DiscreteLocation loc(1);
        const RealVariable x("x"), y("y");
        const InitializerList<Pair<RealVariable,ApproximateDouble>> values{{x,ApproximateDouble(2.0)}};
        SimpleHybridScalings direct(values);
        HybridScalings wrapped(values);
        ARIADNE_TESTING_EXPECT_EQUAL(direct.scaling(loc,x),2.0_x);
        ARIADNE_TESTING_EXPECT_EQUAL(direct.scaling(loc,y),1.0_x);
        ARIADNE_TESTING_EXPECT_EQUAL(wrapped.scaling(loc,y),1.0_x);
        SimpleHybridScalings empty(InitializerList<Pair<RealVariable,ApproximateDouble>>{});
        ARIADNE_TESTING_EXPECT_EQUAL(empty.scaling(loc,x),1.0_x);
        auto pair=x|ApproximateDouble(4.0);
        ARIADNE_TESTING_EXPECT_EQUAL(pair.first.name(),x.name());
        ARIADNE_TESTING_EXPECT_EQUAL(pair.second,4.0_x);
    }
    Void test_setter_contract_and_interface() {
        const DiscreteLocation loc(1);
        const RealVariable x("x");
        SimpleHybridScalings policy;
        policy.set_scaling(x,ApproximateDouble(2.0));
        ARIADNE_TESTING_EXPECT_EXCEPTION(policy.set_scaling(x,ApproximateDouble(0.0)));
        ARIADNE_TESTING_EXPECT_EXCEPTION(policy.set_scaling(x,ApproximateDouble(-1.0)));
        ARIADNE_TESTING_EXPECT_EQUAL(policy.scaling(loc,x),2.0_x);
        HybridScalings wrapped(policy);
        HybridScalingsInterface& mutable_view=wrapped;
        ARIADNE_TESTING_EXPECT_EQUAL(mutable_view.scaling(loc,x),2.0_x);
        const HybridScalings& const_wrapper=wrapped;
        const HybridScalingsInterface& immutable_view=const_wrapper;
        ARIADNE_TESTING_EXPECT_EQUAL(immutable_view.scaling(loc,x),2.0_x);
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_scalings_header");
    ARIADNE_TESTING_ADD_TEST(TestHybridScalings, test_named_and_default_scalings);
    ARIADNE_TESTING_ADD_TEST(TestHybridScalings, test_initializer_scalings);
    ARIADNE_TESTING_ADD_TEST(TestHybridScalings, test_setter_contract_and_interface);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
