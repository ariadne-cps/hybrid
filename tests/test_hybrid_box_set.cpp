/***************************************************************************
 *            tests/test_hybrid_box_set.cpp
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

/** @file tests/test_hybrid_box_set.cpp
 *  @brief Exercise hybrid box set behavior.
 */

// Exercise HybridBoxSet and location-sensitive geometric predicates.
#include <memory>
#include <sstream>
#include <string>

#include "ariadne-testing.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_set.hpp"

using namespace Ariadne;

class TestHybridBoxSet : public Ariadne::Testing::ContextFixture {
public:
    using ContextFixture::ContextFixture;

    Void test_box_set_geometry() {
        DiscreteLocation location(1);
        RealVariable x("x"), y("y");
        RealSpace space({x,y});
        RealVariablesBox coordinates({0<=x<=2,0<=y<=2});
        HybridBoxSet hybrid_box(location,coordinates);

        ARIADNE_TESTING_PROVE(hybrid_box.locations().contains(location));
        ARIADNE_TESTING_EXPECT_EQUAL(hybrid_box.locations().size(),1u);
        ARIADNE_TESTING_PROVE(hybrid_box.variables(location).contains(x));
        ARIADNE_TESTING_PROVE(hybrid_box.variables(location).contains(y));
        ARIADNE_TESTING_EXPECT_EQUAL(hybrid_box.space().dimension(),2u);
        ARIADNE_TESTING_EXPECT_EQUAL(hybrid_box.space(location).dimension(),2u);
        ARIADNE_TESTING_EXPECT_EQUAL(hybrid_box.euclidean_set(space).dimension(),2u);
        // Exercise the location-indexed overload, which preserves the
        // underlying real box when the requested location matches.
        ARIADNE_TESTING_EXPECT_EQUAL(hybrid_box.euclidean_set(location,space).dimension(),2u);
        ARIADNE_TESTING_PROVE(!definitely(hybrid_box.is_empty()));

        // The virtual clone must return a correctly adjusted base pointer
        // even when its implementation has virtual/multiple base classes.
        HybridBoxSet copied_box(hybrid_box);
        ARIADNE_TESTING_PROVE(copied_box.locations().contains(location));
        const EffectiveHybridSetInterface& abstract_box=hybrid_box;
        std::unique_ptr<EffectiveHybridSetInterface> cloned(abstract_box.clone());
        ARIADNE_TESTING_PROVE(cloned.get()!=nullptr);
        ARIADNE_TESTING_PROVE(cloned.get()!=&abstract_box);
        HybridBoxSet* concrete=dynamic_cast<HybridBoxSet*>(cloned.get());
        ARIADNE_TESTING_PROVE(concrete!=nullptr);
        if(concrete==nullptr) { return; }
        ARIADNE_TESTING_EXPECT_EQUAL(cloned->locations().size(),1u);
        ARIADNE_TESTING_PROVE(cloned->locations().contains(location));
        const HybridSetInterfaceBase& root=hybrid_box;
        std::unique_ptr<HybridSetInterfaceBase> root_copy(root.clone());
        ARIADNE_TESTING_PROVE(dynamic_cast<HybridBoxSet*>(root_copy.get())!=nullptr);
        const EffectiveHybridBoundedSetInterface& bounded=hybrid_box;
        std::unique_ptr<EffectiveHybridBoundedSetInterface> bounded_copy(bounded.clone());
        ARIADNE_TESTING_PROVE(dynamic_cast<HybridBoxSet*>(bounded_copy.get())!=nullptr);
        ARIADNE_TESTING_PROVE(hybrid_box.bounding_box().has_location(location));

        std::ostringstream output;
        hybrid_box._write(output);
        ARIADNE_TESTING_PROVE(!output.str().empty());
    }

    Void test_location_semantics() {
        DiscreteLocation location(1), other(2);
        RealVariable x("x"), y("y");
        RealSpace space({x,y});
        HybridBoxSet hybrid_box(location,RealVariablesBox({0<=x<=2,0<=y<=2}));
        HybridExactBox present(location,space,ExactBoxType({{0,1},{0,1}}));
        HybridExactBox absent(other,space,ExactBoxType({{0,1},{0,1}}));

        ARIADNE_TESTING_PROVE(definitely(hybrid_box.separated(absent)));
        ARIADNE_TESTING_PROVE(!definitely(hybrid_box.overlaps(absent)));
        ARIADNE_TESTING_PROVE(!definitely(hybrid_box.covers(absent)));

        // The actual Euclidean predicates are evaluated for the matching
        // location, and yield a well-defined lower logical result.
        static_cast<void>(hybrid_box.separated(present));
        static_cast<void>(hybrid_box.overlaps(present));
        static_cast<void>(hybrid_box.covers(present));
        static_cast<void>(hybrid_box.inside(present));
        static_cast<void>(hybrid_box.inside(absent));

        HybridExactBoxes present_boxes;
        present_boxes.insert(location,space,ExactBoxType({{0,3},{0,3}}));
        ARIADNE_TESTING_PROVE(!definitely(hybrid_box.is_empty()));
        static_cast<void>(hybrid_box.inside(present_boxes));
        HybridExactBoxes absent_boxes;
        absent_boxes.insert(other,space,ExactBoxType({{0,3},{0,3}}));
        static_cast<void>(hybrid_box.inside(absent_boxes));
    }
};

int main(int argc, char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_box_set");
    ARIADNE_TESTING_ADD_TEST(TestHybridBoxSet, test_box_set_geometry);
    ARIADNE_TESTING_ADD_TEST(TestHybridBoxSet, test_location_semantics);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
