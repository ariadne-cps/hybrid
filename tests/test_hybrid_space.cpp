/***************************************************************************
 *            tests/test_hybrid_space.cpp
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

/** @file tests/test_hybrid_space.cpp
 *  @brief Exercise test hybrid space behavior.
 */

// Behavioral coverage for hybrid spaces, location lookup and grids.
#include <memory>
#include <sstream>
#include <string>

#include "hybrid/hybrid_grid.hpp"
#include "hybrid/hybrid_space.hpp"
#include "logging/test_runner.hpp"
#include "paving/grid.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestHybridSpace : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;

    Void test_location_spaces() {
        DiscreteLocation first(1), second(2), missing(3);
        RealVariable x("x"), y("y");
        RealSpace one({x}), two_dimensions({x, y});

        MonolithicHybridSpace m;
        m.new_location(first, one);
        m.new_location(second, two_dimensions);
        ARIADNE_TESTING_PROVE(m.has_location(first));
        ARIADNE_TESTING_PROVE(m.has_location(second));
        ARIADNE_TESTING_PROVE(!m.has_location(missing));
        ARIADNE_TESTING_EXPECT_EQUAL(m[first].dimension(), 1u);
        ARIADNE_TESTING_EXPECT_EQUAL(m[second].dimension(), 2u);
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(m[missing]));
        ARIADNE_TESTING_EXPECT_EQUAL(m.locations().size(), 2u);
        ARIADNE_TESTING_PROVE(m.begin() != m.end());

        MonolithicHybridSpace same;
        same.new_location(first, one);
        same.new_location(second, two_dimensions);
        MonolithicHybridSpace altered;
        altered.new_location(first, two_dimensions);
        altered.new_location(second, two_dimensions);
        MonolithicHybridSpace partial;
        partial.new_location(first, one);
        ARIADNE_TESTING_PROVE(definitely(m.operator==(static_cast<const HybridSpaceInterface&>(same))));
        ARIADNE_TESTING_PROVE(!definitely(m.operator==(static_cast<const HybridSpaceInterface&>(altered))));
        ARIADNE_TESTING_PROVE(!definitely(m.operator==(static_cast<const HybridSpaceInterface&>(partial))));

        Map<DiscreteLocation, RealSpace> entries{
            { first,            one},
            {second, two_dimensions}
        };
        MonolithicHybridSpace from_map(entries);
        ARIADNE_TESTING_EXPECT_EQUAL(from_map[second].dimension(), 2u);
        ARIADNE_TESTING_PROVE(definitely(from_map.operator==(static_cast<const HybridSpaceInterface&>(m))));
        HybridSpace polymorphic(m);
        ARIADNE_TESTING_PROVE(polymorphic.has_location(second));
        ARIADNE_TESTING_EXPECT_EQUAL(polymorphic[second].dimension(), 2u);
        HybridSpace cloned(new MonolithicHybridSpace(m));
        ARIADNE_TESTING_PROVE(cloned.has_location(first));
        ARIADNE_TESTING_PROVE(definitely(polymorphic == cloned));

        std::ostringstream output;
        output << polymorphic;
        ARIADNE_TESTING_PROVE(output.str().find("HybridSpace") != std::string::npos);
    }

    Void test_grid_and_scalings() {
        DiscreteLocation q(1), missing(2);
        RealVariable x("x"), y("y");
        RealSpace spc({x, y});
        MonolithicHybridSpace m;
        m.new_location(q, spc);
        HybridSpace hspc(m);

        HybridGrid unit(hspc);
        ARIADNE_TESTING_PROVE(unit.has_location(q));
        ARIADNE_TESTING_PROVE(!unit.has_location(missing));
        ARIADNE_TESTING_PROVE(unit.space().has_location(q));
        ARIADNE_TESTING_EXPECT_EQUAL(unit.space(q).dimension(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(unit.scalings().scaling(q, x), 1.0_x);
        ARIADNE_TESTING_EXPECT_EQUAL(unit[q].dimension(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(unit.grid(q).dimension(), 2u);

        SimpleHybridScalings policy;
        HybridScalings scalings(policy);
        HybridGrid scaled(hspc, scalings);
        ARIADNE_TESTING_EXPECT_EQUAL(scaled.grid(q).dimension(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(scaled.grid(q).lengths(), unit.grid(q).lengths());
        ARIADNE_TESTING_EXPECT_EQUAL(scaled.space(q).dimension(), 2u);
        // Distinct scaling factors and coordinate orders are materialized by
        // HybridGrid, not by the underlying HybridScalings policy.
        const Map<Identifier, ExactDouble> custom_values{{x.name(), 2.0_x}, {y.name(), 3.0_x}};
        HybridGrid custom_grid(hspc, HybridScalings(custom_values));
        ARIADNE_TESTING_EXPECT_EQUAL(custom_grid[q].dimension(), 2u);
        ARIADNE_TESTING_EXPECT_EQUAL(custom_grid.grid(q).lengths(), custom_grid[q].lengths());
        ARIADNE_TESTING_EXPECT_EQUAL(custom_grid[q].lengths(),
                                     Grid(Vector<ExactDouble>{2.0_x, 3.0_x}).lengths());
        // A derived auxiliary grid may have a different dimension and
        // ordering than the location's state grid. The same scaling policy
        // must be applied by variable name, not by state coordinate index.
        const RealSpace auxiliary_order({y, x});
        const Grid auxiliary_grid =
            HybridGrid::make_grid(custom_grid.scalings(), q, auxiliary_order);
        ARIADNE_TESTING_EXPECT_EQUAL(auxiliary_grid.lengths(),
                                     Grid(Vector<ExactDouble>{3.0_x, 2.0_x}).lengths());
        ARIADNE_TESTING_EXPECT_EXCEPTION(static_cast<void>(custom_grid[missing]));
        std::unique_ptr<HybridGrid> cloned_grid(scaled.clone());
        ARIADNE_TESTING_EXPECT_EQUAL(cloned_grid->grid(q).dimension(), 2u);
        std::ostringstream output;
        output << scaled;
        ARIADNE_TESTING_PROVE(output.str().find("HybridGrid") != std::string::npos);
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_space");
    ARIADNE_TESTING_ADD_TEST(TestHybridSpace, test_location_spaces);
    ARIADNE_TESTING_ADD_TEST(TestHybridSpace, test_grid_and_scalings);
    return Ariadne::Testing::run_logging_suite(ariadne_test_suite, argc, argv);
}
