#include "hybrid/hybrid_augmented_paving.hpp"
#include "function/functional.hpp"

namespace Ariadne {
HybridPaving HybridAugmentedPaving::state_auxiliary_paving() const {
    HybridGrid state_grid=this->state_paving().grid();
    HybridScalings scalings=state_grid.scalings();
    HybridGrid result_grid(this->_auxiliary_mapping.state_auxiliary_space(),scalings);
    HybridPaving result(result_grid);
    for (auto it=this->state_paving().locations_begin(); it!=this->state_paving().locations_end(); ++it) {
        DiscreteLocation const& q=it->first;
        Paving const& paving=it->second;
        LabelledMapping mapping=this->_auxiliary_mapping(q);
        ARIADNE_PRECONDITION(state_grid.space(q)==mapping.argument_space());
        Grid auxiliary_grid=scalings.grid(q,mapping.result_space());
        // Auxiliary product is a derived tree view, not authoritative storage.
        BinaryTreePaving tree(paving.grid());
        for(auto cell=paving.begin(),end=paving.end();cell!=end;++cell)
            tree.adjoin(*cell);
        auto product=outer_skew_product(tree,auxiliary_grid,mapping.mapping());
        result[q]=Paving(new BinaryTreePaving(product));
    }
    return result;
}
} // namespace Ariadne
