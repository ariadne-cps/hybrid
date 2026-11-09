/***************************************************************************
 *            hybrid/hybrid_paving.cpp
 *
 *  Copyright  2008-20  Pieter Collins
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne.
 *
 *  Ariadne is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "function/functional.hpp"

#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_set.hpp"
#include "hybrid/hybrid_paving.hpp"

#include "numeric/real.hpp"

#include "expression-set/expression_set.hpp"
#include "set/function_set.hpp"

#include "hybrid/hybrid_space.hpp"
#include "rendering/figure.hpp"
#include "drawing/binary_tree_paving_draw.hpp"
#include "labelled/figure.hpp"
#include "hybrid/hybrid_graphics.hpp"
#include "numeric/rounding.hpp"
#include "symbolic/assignment.hpp"
#include "labelled/graphics_interface.hpp"
#include "set/function_set.hpp"

namespace Ariadne {





Void HybridPaving::clear() {
    for(HybridPaving::LocationsIterator _loc_iter=this->locations_begin(); _loc_iter!=this->locations_end(); ++_loc_iter) {
        _loc_iter->second.clear();
    }
}

Void HybridPaving::adjoin(const HybridGridCell& hgc) {
    this->_provide_location(hgc.location()).adjoin(hgc.euclidean_set());
}

Void HybridPaving::adjoin(const ListSet<HybridGridCell>& hgcls) {
    for(ListSet<HybridGridCell>::ConstIterator iter=hgcls.begin(); iter!=hgcls.end(); ++iter) {
        this ->adjoin(*iter);
    }
}

Void HybridPaving::adjoin(const HybridPaving& hgts) {
    for(HybridPaving::LocationsConstIterator _loc_iter=hgts.locations_begin(); _loc_iter!=hgts.locations_end(); ++_loc_iter) {
        this->_provide_location(_loc_iter->first).adjoin(_loc_iter->second);
    }
}

Void HybridPaving::remove(const HybridPaving& hgts) {
    for(HybridPaving::LocationsConstIterator _loc_iter=hgts.locations_begin(); _loc_iter!=hgts.locations_end(); ++_loc_iter) {
        this->_provide_location(_loc_iter->first).remove(_loc_iter->second);
    }
}

Void HybridPaving::restrict(const HybridPaving& hgts) {
    // Intersection must also clear left-hand locations absent on the right.
    // Iterating the left map preserves its locations and cannot introduce
    // unrelated locations from the restricting paving.
    for(LocationsIterator iter=this->locations_begin(); iter!=this->locations_end(); ++iter) {
        auto other=hgts._map.find(iter->first);
        if(other==hgts._map.end()) iter->second.clear();
        else iter->second.restrict(other->second);
    }
}

Void HybridPaving::restrict_to_extent(Nat h) {
    for(LocationsIterator _loc_iter=locations_begin(); _loc_iter!=locations_end(); ++_loc_iter) {
        _loc_iter->second.restrict_to_extent(h);
    }
}

Void HybridPaving::adjoin_inner_approximation(const EffectiveHybridSetInterface& hset, const Nat fineness) {
    Set<DiscreteLocation> locations=hset.locations();
    for(auto location : locations) {
        RealSpace space = this->space(location);
        this->_provide_location(location).adjoin_inner_approximation(hset.euclidean_set(location,space),fineness);
    }
}

Void HybridPaving::adjoin_inner_approximation(const HybridExactBoxes& hbxs, const Nat fineness) {
    for(HybridExactBoxes::ConstIterator _loc_iter=hbxs.begin();
            _loc_iter!=hbxs.end(); ++_loc_iter) {
        DiscreteLocation const& loc=_loc_iter->first;
        LabelledSet<ExactBoxType> const& vbx=_loc_iter->second;
        ARIADNE_PRECONDITION(vbx.space() == this->space(loc));
        this->_provide_location(loc).adjoin_inner_approximation(vbx.euclidean_set(),fineness);
    }
}

Void HybridPaving::adjoin_lower_approximation(const EffectiveHybridOvertSetInterface& hs, const Nat extent, const Nat fineness) {
    Set<DiscreteLocation> hlocs=dynamic_cast<const EffectiveHybridBoundedSetInterface&>(hs).locations();
    for(Set<DiscreteLocation>::ConstIterator _loc_iter=hlocs.begin();
            _loc_iter!=hlocs.end(); ++_loc_iter) {
        DiscreteLocation loc=*_loc_iter;
        RealSpace spc=this->space(loc);
        this->_provide_location(loc).adjoin_lower_approximation(hs.euclidean_set(loc,spc),extent,fineness);
    }
}

Void HybridPaving::adjoin_outer_approximation(const EffectiveHybridCompactSetInterface& hs, const Nat fineness) {
    Set<DiscreteLocation> hlocs=hs.locations();
    for(Set<DiscreteLocation>::ConstIterator _loc_iter=hlocs.begin();
            _loc_iter!=hlocs.end(); ++_loc_iter) {
        DiscreteLocation loc=*_loc_iter;
        RealSpace spc=this->space(loc);
        this->_provide_location(loc).adjoin_outer_approximation(hs.euclidean_set(loc,spc),fineness);
    }
}

Void HybridPaving::adjoin_outer_approximation(const HybridExactBoxes& hbxs, const Nat fineness) {
    for(HybridExactBoxes::ConstIterator _loc_iter=hbxs.begin();
            _loc_iter!=hbxs.end(); ++_loc_iter) {
        DiscreteLocation const& loc=_loc_iter->first;
        LabelledSet<ExactBoxType> const& vbx=_loc_iter->second;
        ARIADNE_PRECONDITION(vbx.space() == this->space(loc));
        this->_provide_location(_loc_iter->first).adjoin_outer_approximation(vbx.euclidean_set(),fineness);
    }
}


Paving& HybridPaving::operator[](DiscreteLocation q) {
    return this->_provide_location(q);
}

const Paving& HybridPaving::operator[](DiscreteLocation q) const {
    ARIADNE_PRECONDITION_MSG(this->has_location(q),"q="<<q);
    auto found=_map.find(q);
    ARIADNE_PRECONDITION_MSG(found!=_map.end(),"Location has no materialized paving: "<<q);
    return found->second;
}

Bool HybridPaving::is_empty() const {
    for(LocationsConstIterator _loc_iter=this->locations_begin();
        _loc_iter!=this->locations_end(); ++_loc_iter) {
        if(!_loc_iter->second.is_empty()) { return false; }
    }
    return true;
}

SizeType HybridPaving::size() const {
    SizeType result=0;
    for(LocationsConstIterator _loc_iter=this->locations_begin(); _loc_iter!=this->locations_end(); ++_loc_iter) {
        result+=_loc_iter->second.size();
    }
    return result;
}

HybridListSet<ExactBoxType> HybridPaving::boxes() const {
    HybridListSet<ExactBoxType> result;
    for(ConstIterator iter=this->begin(); iter!=this->end(); ++iter) {
        result.adjoin(iter->location(),iter->space(),iter->euclidean_set().box());
    }
    return result;
}

Void HybridPaving::mince(Nat fineness) {
    for(LocationsIterator _loc_iter=this->locations_begin();
        _loc_iter!=this->locations_end(); ++_loc_iter) {
        _loc_iter->second.mince(fineness);
    }
}

Void HybridPaving::recombine() {
    for(LocationsIterator _loc_iter=this->locations_begin();
        _loc_iter!=this->locations_end(); ++_loc_iter) {
        _loc_iter->second.recombine();
    }
}

ValidatedLowerKleenean HybridPaving::separated(const HybridExactBox& hbx) const {
    LocationsConstIterator loc_iter = this->_map.find(hbx.location());
    // ValidatedLowerKleenean overloads operator||, so it does not short-circuit.
    if (loc_iter == this->locations_end()) { return true; }
    return loc_iter->second.separated(hbx.euclidean_set());
}

ValidatedLowerKleenean HybridPaving::overlaps(const HybridExactBox& hbx) const {
    LocationsConstIterator loc_iter = this->_map.find(hbx.location());
    // Do not evaluate the predicate on an absent location (overloaded &&).
    if (loc_iter == this->locations_end()) { return false; }
    return loc_iter->second.overlaps(hbx.euclidean_set());
}

ValidatedLowerKleenean HybridPaving::covers(const HybridExactBox& hbx) const {
    LocationsConstIterator loc_iter = this->_map.find(hbx.location());
    if (loc_iter == this->locations_end()) { return false; }
    return loc_iter->second.superset(hbx.euclidean_set());
}

ValidatedLowerKleenean HybridPaving::inside(const HybridExactBoxes& hbx) const  {
    // All nonempty locations must be included. A box collection missing
    // a nonempty location cannot contain the whole hybrid paving.
    ValidatedLowerKleenean included=true;
    for(LocationsConstIterator loc_iter=this->locations_begin(); loc_iter!=this->locations_end(); ++loc_iter) {
        if(!loc_iter->second.is_empty()) {
            DiscreteLocation const& loc=loc_iter->first;
            if(!hbx.has_location(loc)) { return false; }
            RealSpace spc=this->space(loc);
            included = included && loc_iter->second.inside(hbx.euclidean_set(loc,spc));
        }
    }
    return included;
}

HybridUpperBoxes HybridPaving::bounding_box() const {
    HybridUpperBoxes result;
    for( LocationsConstIterator _loc_iter = this->locations_begin(); _loc_iter != this->locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace const& spc=this->space(loc);
            result.insert(loc,spc,_loc_iter->second.bounding_box());
        }
    }
    return result;
}

Bool subset(const HybridPaving& hgts1, const HybridPaving& hgts2) {
    for( typename HybridPaving::LocationsConstIterator _loc_iter = hgts1.locations_begin(); _loc_iter != hgts1.locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace spc=hgts1.space(loc);
            // A missing right-hand slice denotes the empty set.
            if(!hgts2.nontrivial_location(loc) || not hgts1.euclidean_set(loc).subset(
                    static_cast<PavingInterface const&>(hgts2.euclidean_set(loc,spc)))) {
                return false;
            }
        }
    }
    return true;
}

Bool intersect(const HybridPaving& hgts1, const HybridPaving& hgts2) {
    for( typename HybridPaving::LocationsConstIterator _loc_iter = hgts1.locations_begin(); _loc_iter != hgts1.locations_end(); ++_loc_iter ) {
        if( !_loc_iter->second.is_empty() ) {
            DiscreteLocation const& loc = _loc_iter->first;
            RealSpace spc=hgts1.space(loc);
            if(hgts2.nontrivial_location(loc) && hgts1.euclidean_set(loc).intersects(
                    static_cast<PavingInterface const&>(hgts2.euclidean_set(loc,spc)))) {
                return true;
            }
        }
    }
    return false;
}

OutputStream& HybridPaving::_write(OutputStream& os) const {
    return os << this->_map;
}

Void HybridPaving::draw(CanvasInterface& canvas, const Set<DiscreteLocation>& locations, const Variables2d& axis_variables) const {
    for(LocationsConstIterator loc_iter=this->locations_begin(); loc_iter!=this->locations_end(); ++loc_iter) {
        if(locations.empty() || locations.contains(loc_iter->first)) {
            RealSpace const& space=this->space(loc_iter->first);
            Projection2d projection(space.dimension(),space.index(axis_variables.x()),space.index(axis_variables.y()));
            // Drawing creates a tree view; authoritative backend is unchanged.
            BinaryTreePaving tree(loc_iter->second.grid());
            for(auto it=loc_iter->second.begin(),end=loc_iter->second.end();it!=end;++it)
                tree.adjoin(*it);
            Ariadne::draw(canvas,projection,tree);
        }
    }
}


Paving& HybridPaving::_provide_location(DiscreteLocation const& q) {
    ARIADNE_PRECONDITION_MSG(_hgrid.has_location(q),
        "HybridPaving location does not belong to grid");
    auto found=_map.find(q);
    if(found==_map.end()) {
        Paving selected=_factory(_hgrid[q]);
        ARIADNE_PRECONDITION_MSG(selected.grid()==_hgrid[q],
            "Selected backend uses an incompatible grid");
        _map.insert(std::make_pair(q,std::move(selected)));
        found=_map.find(q);
    }
    return found->second;
}


Grid HybridScalings::grid(const DiscreteLocation& loc, const RealSpace& space) const
{
    Vector<ExactDouble> lengths(space.size());
    for(SizeType i=0; i!=space.size(); ++i) {
        lengths[i] = (this->scaling(loc,space.variable(i)));
    }
    return Grid(lengths);
}

} // namespace Ariadne
