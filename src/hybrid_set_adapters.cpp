/***************************************************************************
 *            hybrid/hybrid_set.cpp
 *
 *  Copyright  2008-20  Pieter Collins
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Hybrid.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file src/hybrid_set_adapters.cpp
 *  @brief Adapt hybrid sets to supported representations.
 */

#include <type_traits>
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_set.hpp"
#include "hybrid/hybrid_paving.hpp"
#include "hybrid/hybrid_graphics.hpp"
#include "drawing/geometry_draw.hpp"

namespace Ariadne {

Void HybridBoxSet::draw(CanvasInterface& c, const Set<DiscreteLocation>& qs, const Variables2d& vs) const {
    if(qs.empty() || qs.contains(this->location())) {
        RealSpace spc(List<RealVariable>(this->variables()));
        Projection2d prj(spc.dimension(),spc.index(vs.x()),spc.index(vs.y()));
        Ariadne::draw(c,prj,this->euclidean_set(spc));
    }
}




Void HybridBoundedConstraintSet::draw(CanvasInterface& c, const Set<DiscreteLocation>& q, const Variables2d& p) const {
    for(auto loc : this->locations()) {
        if(q.empty() || q.contains(loc)) {
            Set<RealVariable> variables=this->variables(loc);
            RealSpace space(List<RealVariable>(variables.begin(),variables.end()));
            Ariadne::draw(c,projection(space,p),this->euclidean_set(loc,space));
        }
    }
}


template<class EBS> Void HybridBasicSet<EBS>::adjoin_outer_approximation_to(HybridPaving& paving, Nat fineness) const {
    ARIADNE_PRECONDITION_MSG(this->space()==paving.space(this->location()), "HybridSet's state variables "<<this->space()<<" do not match "<<paving.space(this->location())<<" in location "<<this->location());
    if constexpr (std::is_same_v<EBS,ExactBoxType>) {
        // Explicitly use the effective compact-set interface rather than
        // relying on a backend-specific exact-box overload.
        paving[this->location()].adjoin_outer_approximation(
            ExactBoxSetType(this->euclidean_set()),fineness);
    } else {
        paving[this->location()].adjoin_outer_approximation(
            this->euclidean_set(),fineness);
    }
}


template<class BS> Void draw_hybrid_basic_set(CanvasInterface& canvas, const DiscreteLocation& location, const Variables2d& axes, const HybridBasicSet<BS>& set)
{
    if(set.location()==location) {
        Projection2d projection(set.euclidean_set().dimension(),set.space().index(axes.x()),set.space().index(axes.y()));
        Ariadne::draw(canvas,projection,set.euclidean_set());
    }
}

template<class EBS> Void HybridBasicSet<EBS>::draw(CanvasInterface& canvas, const Set<DiscreteLocation>& locations, const Variables2d& axes) const
{
    // An empty selection means every location, as for all other hybrid
    // drawables and as specified by HybridBasicSet's public contract.
    if (locations.empty()) {
        draw_hybrid_basic_set(canvas,this->location(),axes,*this);
    } else {
        for (auto loc : locations) draw_hybrid_basic_set(canvas,loc,axes,*this);
    }
}


template<class IVL> Void HybridBox<IVL>::draw(CanvasInterface& c, const Set<DiscreteLocation>& qs, const Variables2d& p) const {
    DiscreteLocation const& q=this->location();
    if(qs.empty() || qs.contains(q)) {
        Ariadne::draw_hybrid_basic_set(c,q,p,static_cast<HybridBasicSet<Box<IVL>>const&>(*this));
    }
}

template<class IVL> Void HybridBoxes<IVL>::draw(CanvasInterface& canvas, const Set<DiscreteLocation>& locations, const Variables2d& axis_variables) const {
    for(auto loc_iter=this->begin(); loc_iter!=this->end(); ++loc_iter) {
        if(locations.empty() || locations.contains(loc_iter->first)) {
            RealSpace const& space=this->space(loc_iter->first);
            Projection2d projection(space.dimension(),space.index(axis_variables.x()),space.index(axis_variables.y()));
            Ariadne::draw(canvas,projection,loc_iter->second.euclidean_set());
        }
    }
}



// Instantiations

template class HybridBasicSet<ExactBoxType>;
template class HybridBox<ExactIntervalType>;
template class HybridBoxes<ExactIntervalType>;

template class HybridBasicSet<UpperBoxType>;
template class HybridBox<UpperIntervalType>;
template class HybridBoxes<UpperIntervalType>;

template class HybridBox<RealInterval>;

// The drawable interface delegates to this out-of-line template member.
template Void HybridBasicSet<ValidatedConstrainedImageSet>::draw(
    CanvasInterface&, const Set<DiscreteLocation>&, const Variables2d&) const;



} // namespace Ariadne
