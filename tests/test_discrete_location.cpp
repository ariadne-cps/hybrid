/***************************************************************************
 *            tests/test_discrete_location.cpp
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

/** @file tests/test_discrete_location.cpp
 *  @brief Exercise discrete location behavior.
 */

// Targeted behavioral coverage for hybrid discrete locations.
// Tests public contracts rather than constructing invalid internal states.
#include "hybrid/discrete_location.hpp"
#include "symbolic/valuation.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestDiscreteLocation : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_construction_and_combination() {
        StringVariable a("a"), b("b");
        String zero("zero"), one("one");

        DiscreteLocation empty;
        DiscreteLocation left(a, zero);
        DiscreteLocation right(b, one);
        DiscreteLocation combined = join(left, right);

        ARIADNE_TESTING_PROVE(are_same(empty, DiscreteLocation()));
        ARIADNE_TESTING_PROVE(is_restriction(left, combined));
        ARIADNE_TESTING_PROVE(is_restriction(right, combined));
        ARIADNE_TESTING_PROVE(!are_same(left, combined));

        DiscreteLocation appended(left);
        appended.adjoin(right);
        ARIADNE_TESTING_PROVE(are_same(appended, combined));

        Map<StringVariable, String> variable_map;
        variable_map.insert(a, zero);
        DiscreteLocation from_variable_map(variable_map);
        ARIADNE_TESTING_PROVE(are_same(from_variable_map, left));

        Map<Identifier, String> identifier_map;
        identifier_map.insert(Identifier("a"), zero);
        DiscreteLocation from_identifier_map(identifier_map);
        ARIADNE_TESTING_PROVE(are_same(from_identifier_map, left));

        StringValuation valuation(variable_map);
        DiscreteLocation from_valuation(valuation);
        ARIADNE_TESTING_PROVE(are_same(from_valuation, left));

        DiscreteLocation from_initializer({a | zero, b | one});
        ARIADNE_TESTING_PROVE(are_same(from_initializer, combined));

        // The valid call retains every supplied key. The current implementation
        // requires the argument set to include the location's variable set.
        Set<Identifier> keys = {Identifier("a"), Identifier("b")};
        DiscreteLocation retained = restrict(combined, keys);
        ARIADNE_TESTING_PROVE(are_same(retained, combined));
        Set<Identifier> only_a = {Identifier("a")};
        DiscreteLocation projected = restrict(combined, only_a);
        ARIADNE_TESTING_PROVE(are_same(projected, left));
        Set<Identifier> none;
        ARIADNE_TESTING_PROVE(are_same(restrict(combined, none), empty));
    }

    Void test_equality_and_order() {
        StringVariable a("a"), b("b");
        DiscreteLocation a0(a, String("0"));
        DiscreteLocation a1(a, String("1"));
        DiscreteLocation b0(b, String("0"));
        DiscreteLocation a0b0({a | String("0"), b | String("0")});
        DiscreteLocation a0b1({a | String("0"), b | String("1")});
        DiscreteLocation a1b0({a | String("1"), b | String("0")});

        ARIADNE_TESTING_PROVE(a0 == a0);
        ARIADNE_TESTING_PROVE(a0 != a1);
        ARIADNE_TESTING_PROVE(a0 < a1);
        ARIADNE_TESTING_PROVE(!(a1 < a0));
        ARIADNE_TESTING_PROVE(a0 < b0);
        ARIADNE_TESTING_PROVE(!(b0 < a0));
        ARIADNE_TESTING_PROVE(a0b0 < a0b1);
        ARIADNE_TESTING_PROVE(!(a0b1 < a0b0));
        ARIADNE_TESTING_PROVE(a0b0 < a1b0);
        ARIADNE_TESTING_PROVE(a0 < a0b0);
        ARIADNE_TESTING_PROVE(!(a0b0 < a0));

        // A common variable with different values is ordinary inequality.
        ARIADNE_TESTING_PROVE(!(a0b0 == a0b1));

        // Distinct or partially specified valuations with no value conflict
        // cannot be compared for equality: the API explicitly throws.
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(a0 == b0));
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(b0 == a0));
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(a0 == a0b0));
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(a0b0 == a0));
    }

    Void test_distinguishability_and_restriction() {
        StringVariable a("a"), b("b"), c("c");
        DiscreteLocation empty;
        DiscreteLocation a0(a, String("0"));
        DiscreteLocation a1(a, String("1"));
        DiscreteLocation b0(b, String("0"));
        DiscreteLocation c0(c, String("0"));
        DiscreteLocation full({a | String("0"), b | String("0")});

        ARIADNE_TESTING_PROVE(are_distinguishable(a0, a1));
        ARIADNE_TESTING_PROVE(!are_distinguishable(empty, a0));
        ARIADNE_TESTING_PROVE(!are_distinguishable(a0, empty));
        ARIADNE_TESTING_PROVE(!are_distinguishable(a0, b0));
        ARIADNE_TESTING_PROVE(!are_distinguishable(a0, full));
        ARIADNE_TESTING_PROVE(!are_same(a0, a1));
        ARIADNE_TESTING_PROVE(are_same(a0, a0));

        ARIADNE_TESTING_PROVE(is_restriction(empty, full));
        ARIADNE_TESTING_PROVE(is_restriction(a0, full));
        ARIADNE_TESTING_PROVE(is_restriction(full, full));
        ARIADNE_TESTING_PROVE(!is_restriction(a1, full));
        ARIADNE_TESTING_PROVE(!is_restriction(c0, full));
        ARIADNE_TESTING_PROVE(!is_restriction(full, a0));
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_discrete_location");
    ARIADNE_TESTING_ADD_TEST(TestDiscreteLocation, test_construction_and_combination);
    ARIADNE_TESTING_ADD_TEST(TestDiscreteLocation, test_equality_and_order);
    ARIADNE_TESTING_ADD_TEST(TestDiscreteLocation, test_distinguishability_and_restriction);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
