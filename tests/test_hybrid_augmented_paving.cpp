#include <functional>
#include <sstream>
#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_augmented_paving.hpp"
#include "hybrid/hybrid_augmented_paving_draw.hpp"
#include "labelled/figure.hpp"
#include "rendering/null_rendering.hpp"
#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestHybridAugmentedPaving : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;
    Void test_augmented_paving_mutation_and_views() {
        DiscreteLocation q(1),vacant(2);
        RealVariable x("x"),z("z"),wrong("wrong");
        RealSpace state({x}),auxiliary({z});
        MonolithicHybridSpace states,full;
        states.new_location(q,state);
        states.new_location(vacant,state);
        full.new_location(q,join(state,auxiliary));
        full.new_location(vacant,join(state,auxiliary));
        Mapping function=Mapping::identity(1u);
        LabelledMapping labelled(state,function,auxiliary);
        HybridAuxiliaryMapping mapping(HybridSpace(full),
            [labelled](DiscreteLocation const&) { return labelled; });
        HybridGrid grid(states);
        HybridBinaryTreePaving source(grid);
        HybridExactBoxes boxes;
        boxes.insert(q,state,ExactBoxType({{0.125_x,0.875_x}}));
        source.adjoin_outer_approximation(boxes,3u);
        HybridAugmentedPaving paving(source,mapping),empty(grid,mapping);
        HybridAugmentedPaving const& immutable=paving;
        ARIADNE_TESTING_EXPECT_EQUAL(&immutable.state_set(),&immutable.state_paving());
        ARIADNE_TESTING_PROVE(immutable.grid().has_location(q));
        ARIADNE_TESTING_EXPECT_EQUAL(immutable.auxiliary_data()(q).result_space(),auxiliary);
        ARIADNE_TESTING_PROVE(immutable.begin()!=immutable.end());
        ARIADNE_TESTING_EXPECT_EQUAL(immutable[q].state_space(),state);
        LabelledAugmentedPaving slice=paving[q];
        ARIADNE_TESTING_PROVE(!slice.is_empty());
        empty.adjoin(paving);
        ARIADNE_TESTING_PROVE(subset(paving,empty));
        ARIADNE_TESTING_PROVE(intersect(paving,empty));
        empty.remove(paving);
        ARIADNE_TESTING_PROVE(empty.is_empty());
        empty.adjoin(paving);
        empty.restrict(paving);
        ARIADNE_TESTING_PROVE(subset(paving,empty));
        empty.mince(4u);
        empty.recombine();
        empty.restrict_to_extent(2u);
        ARIADNE_TESTING_PROVE(!empty.is_empty());
        empty[q].clear();
        ARIADNE_TESTING_PROVE(empty.is_empty());
        empty.clear();
        ARIADNE_TESTING_PROVE(empty.is_empty());
        HybridBoundedConstraintSet region(q,{0<=x<=1});
        empty.adjoin_outer_approximation(region,3u);
        ARIADNE_TESTING_PROVE(!empty.is_empty());
        HybridAugmentedPaving lower(grid,mapping);
        lower.adjoin_lower_approximation(region,1u,3u);
        ARIADNE_TESTING_PROVE(!lower.is_empty());
        std::ostringstream text;
        text<<paving;
        ARIADNE_TESTING_PROVE(text.str().find("HybridAugmentedPaving")!=std::string::npos);
        LabelledMapping incompatible(RealSpace({wrong}),function,auxiliary);
        HybridAuxiliaryMapping wrong_mapping(HybridSpace(full),
            [incompatible](DiscreteLocation const&) { return incompatible; });
        HybridAugmentedPaving invalid(source,wrong_mapping);
        ARIADNE_TESTING_EXPECT_EXCEPTION(invalid.state_auxiliary_paving());
    }
    Void test_lazy_auxiliary_and_drawing() {
        DiscreteLocation q(1);
        RealVariable x("x"),y("y"),z("z");
        RealSpace state({x,y}), auxiliary({z});
        MonolithicHybridSpace state_space;
        state_space.new_location(q,state);
        MonolithicHybridSpace full_space;
        full_space.new_location(q,join(state,auxiliary));
        EffectiveVectorMultivariateFunction function({
            EffectiveScalarMultivariateFunction::coordinate(2u,0u)});
        LabelledMapping labelled(state,function,auxiliary);
        HybridAuxiliaryMapping auxiliary_mapping(HybridSpace(full_space),
            [labelled](DiscreteLocation const&) { return labelled; });
        HybridAugmentedPaving paving(HybridGrid(state_space),auxiliary_mapping);
        ARIADNE_TESTING_PROVE(paving.is_empty());
        ARIADNE_TESTING_PROVE(&paving.state_set()==&paving.state_paving());
        HybridExactBoxes boxes;
        boxes.insert(q,state,ExactBoxType({{-1,1},{-1,1}}));
        paving.adjoin_outer_approximation(boxes,2u);
        ARIADNE_TESTING_PROVE(paving.size()>0u);
        ARIADNE_TESTING_PROVE(paving.state_auxiliary_paving().size()>0u);
        ARIADNE_TESTING_EXPECT_EQUAL(paving.auxiliary_mapping(q).argument_space(),state);
        NullCanvas canvas;
        HybridAugmentedPavingDrawable drawable(paving);
        drawable.draw(canvas,Set<DiscreteLocation>{q},Variables2d(x,y));
    }
};

int main(int argc,char** argv) {
    ARIADNE_TESTING_SUITE("test_hybrid_augmented_paving");
    ARIADNE_TESTING_ADD_TEST(TestHybridAugmentedPaving,test_lazy_auxiliary_and_drawing);
    ARIADNE_TESTING_ADD_TEST(TestHybridAugmentedPaving,test_augmented_paving_mutation_and_views);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc,argv);
}
