/***************************************************************************
 *            tests/test_value.cpp
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

/** @file tests/test_value.cpp
 *  @brief Verify generic located values and hybrid lazy families.
 */

#include <string>

#include "hybrid/value.hpp"
#include "logging/test_runner.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestHybridValue : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_located() {
        const DiscreteLocation q(1);
        Located<std::string> item(q, "content");
        ARIADNE_TESTING_PROVE(item.location() == q);
        ARIADNE_TESTING_EXPECT_EQUAL(item.value(), std::string("content"));
    }

    Void test_explicit_family() {
        const DiscreteLocation q1(1);
        const DiscreteLocation q2(2);
        Hybrid<int> items;
        items.insert(q1, 17);
        ARIADNE_TESTING_PROVE(items.has_location(q1));
        ARIADNE_TESTING_PROVE(not items.has_location(q2));
        ARIADNE_TESTING_EXPECT_EQUAL(items[q1], 17);
        ARIADNE_TESTING_EXPECT_EQUAL(items.size(), 1u);
    }

    Void test_lazy_family() {
        MonolithicHybridSpace spaces;
        const DiscreteLocation q1(1);
        const DiscreteLocation q2(2);
        const DiscreteLocation invalid(3);
        spaces.new_location(q1, RealSpace({RealVariable("x")}));
        spaces.new_location(q2, RealSpace({RealVariable("x"), RealVariable("y")}));

        SizeType constructions = 0u;
        Hybrid<SizeType> items(
            HybridSpace(spaces),
            [&constructions](const DiscreteLocation&, const RealSpace& space) {
                ++constructions;
                return space.dimension();
            });

        ARIADNE_TESTING_PROVE(items.has_location(q2));
        ARIADNE_TESTING_PROVE(not items.has_value(q2));
        ARIADNE_TESTING_EXPECT_EQUAL(items.size(), 0u);
        ARIADNE_TESTING_EXPECT_EQUAL(items[q2], 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(constructions, 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(items[q2], 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(constructions, 1u);
        ARIADNE_TESTING_PROVE(items.has_value(q2));
        ARIADNE_TESTING_PROVE(items.has_location(q1));
        ARIADNE_TESTING_PROVE(not items.has_location(invalid));
        ARIADNE_TESTING_EXPECT_EQUAL(items.size(), 1u);

        const auto& const_items = items;
        ARIADNE_TESTING_EXPECT_EQUAL(const_items[q2], 2u);
        ARIADNE_TESTING_EXPECT_EXCEPTION(const_items[q1]);
        ARIADNE_TESTING_EXPECT_EQUAL(items.materialized_locations().size(), 1u);
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_value");
    ARIADNE_TESTING_ADD_TEST(TestHybridValue, test_located);
    ARIADNE_TESTING_ADD_TEST(TestHybridValue, test_explicit_family);
    ARIADNE_TESTING_ADD_TEST(TestHybridValue, test_lazy_family);
    return Ariadne::Testing::run_logging_suite(ariadne_test_suite, argc, argv);
}
