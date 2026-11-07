#include <functional>
#include "hybrid/hybrid_augmented_paving.hpp"
#include "hybrid/hybrid_augmented_paving_draw.hpp"
#include "rendering/null_rendering.hpp"
#include "ariadne-testing.hpp"

using namespace Ariadne;

class TestHybridAugmentedPaving : public Ariadne::Testing::ContextFixture {
  public:
    using ContextFixture::ContextFixture;
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
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc,argv);
}
