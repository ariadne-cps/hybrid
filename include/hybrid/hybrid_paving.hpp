/***************************************************************************
 *            hybrid/hybrid_paving.hpp
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

/*! \file hybrid/hybrid_paving.hpp
 *  \brief Hybrid extension of a paving.
 */

#ifndef ARIADNE_HYBRID_PAVING_HPP
#define ARIADNE_HYBRID_PAVING_HPP

#include <map>
#include <functional>
#include <utility>


#include <memory>

#include "utility/macros.hpp"
#include "utility/stlio.hpp"
#include "numeric/declarations.hpp"
#include "algebra/declarations.hpp"
#include "function/declarations.hpp"
#include "geometry/declarations.hpp"
#include "utility/container.hpp"
#include "set/function_set.hpp"
#include "set/list_set.hpp"
#include "paving/binary_tree_paving.hpp"
#include "paving/robdd_paving.hpp"
#include "set/curve.hpp"

#include "expression-set/expression_set.hpp"

#include "hybrid/hybrid_set.decl.hpp"
#include "hybrid/hybrid_set_interface.hpp"
#include "hybrid/hybrid_expression_set.hpp"
#include "hybrid/hybrid_space.hpp"
#include "hybrid/hybrid_grid.hpp"
#include "hybrid/hybrid_set.hpp"
#include "geometry/point.hpp"
#include "geometry/box.hpp"

#include "hybrid/hybrid_graphics_interface.hpp"

namespace Ariadne {


//! \ingroup HybridSetSubModule
//! A backend-independent location-indexed paving of hybrid states.
class HybridPaving : public HybridDrawableInterface {
  public:
    using Factory=std::function<Paving(Grid const&)>;
    using GridType=HybridGrid;
    using LocationsIterator=Map<DiscreteLocation,Paving>::Iterator;
    using LocationsConstIterator=Map<DiscreteLocation,Paving>::ConstIterator;
    using ConstIterator=HybridSpaceSetConstIterator<Paving,HybridGridCell>;
    explicit HybridPaving(HybridGrid const& grid)
      : HybridPaving(grid,[](Grid const& g){return Paving(new BinaryTreePaving(g));}) { }
    HybridPaving(HybridGrid const& grid,Factory factory)
      : _hgrid(grid),_map(),_factory(std::move(factory)) {
        ARIADNE_PRECONDITION_MSG(static_cast<Bool>(_factory),"Empty HybridPaving factory");
    }
    HybridPaving(HybridGrid const& grid,std::shared_ptr<RobddManager> manager)
      : HybridPaving(grid,[manager=std::move(manager)](Grid const& g) {
        return Paving(new RobddPaving(manager,g));
    }) { }
    HybridPaving* clone() const { return new HybridPaving(*this); }
    // Create another hybrid paving on a different hybrid grid while
    // preserving the selected backend factory and shared ROBDD manager.
    HybridPaving with_grid(HybridGrid const& grid) const {
        return HybridPaving(grid,_factory);
    }
    HybridGrid grid() const { return _hgrid; }
    HybridSpace space() const { return _hgrid.space(); }
    RealSpace space(DiscreteLocation q) const { return _hgrid.space(q); }
    Bool has_location(DiscreteLocation q) const { return _hgrid.has_location(q); }
    Bool nontrivial_location(DiscreteLocation q) const { return _map.has_key(q); }
    LocationsIterator locations_begin() { return _map.begin(); }
    LocationsIterator locations_end() { return _map.end(); }
    LocationsConstIterator locations_begin() const { return _map.begin(); }
    LocationsConstIterator locations_end() const { return _map.end(); }
    ConstIterator begin() const { return ConstIterator(_map,_hgrid.space(),false); }
    ConstIterator end() const { return ConstIterator(_map,_hgrid.space(),true); }
    Paving& operator[](DiscreteLocation q);
    Paving const& operator[](DiscreteLocation q) const;
    Paving const& euclidean_set(DiscreteLocation q) const { return (*this)[q]; }
    Paving const& euclidean_set(DiscreteLocation q,RealSpace const& s) const {
        ARIADNE_PRECONDITION_MSG(s==space(q),"Wrong variable ordering in "<<q);
        return (*this)[q];
    }
    Void adjoin(HybridGridCell const&);
    Void adjoin(ListSet<HybridGridCell> const&);
    Void adjoin(HybridPaving const&);
    Void remove(HybridPaving const&);
    Void restrict(HybridPaving const&);
    Void clear();
    Void restrict_to_extent(Nat);
    Void adjoin_inner_approximation(HybridExactBoxes const&,Nat);
    Void adjoin_inner_approximation(EffectiveHybridSetInterface const&,Nat);
    Void adjoin_lower_approximation(EffectiveHybridOvertSetInterface const&,Nat,Nat);
    Void adjoin_outer_approximation(EffectiveHybridCompactSetInterface const&,Nat);
    Void adjoin_outer_approximation(HybridExactBoxes const&,Nat);
    Bool is_empty() const;
    SizeType size() const;
    HybridListSet<ExactBoxType> boxes() const;
    Void mince(Nat);
    Void recombine();
    ValidatedLowerKleenean separated(HybridExactBox const&) const;
    ValidatedLowerKleenean overlaps(HybridExactBox const&) const;
    ValidatedLowerKleenean covers(HybridExactBox const&) const;
    ValidatedLowerKleenean inside(HybridExactBoxes const&) const;
    HybridUpperBoxes bounding_box() const;
    OutputStream& _write(OutputStream&) const;
    Void draw(CanvasInterface&,Set<DiscreteLocation> const&,Variables2d const&) const override;
    friend Bool subset(HybridPaving const&,HybridPaving const&);
    friend Bool intersect(HybridPaving const&,HybridPaving const&);
    friend OutputStream& operator<<(OutputStream& os,HybridPaving const& p) {
        return os<<"HybridPaving("<<p._map<<")";
    }
  private:
    Paving& _provide_location(DiscreteLocation const&);
    HybridGrid _hgrid;
    Map<DiscreteLocation,Paving> _map;
    Factory _factory;
};

inline HybridPaving inner_approximation(
        EffectiveHybridSetInterface const& set,HybridGrid const& grid,Nat fineness) {
    HybridPaving result(grid);
    result.adjoin_inner_approximation(set,fineness);
    return result;
}
} // namespace Ariadne
#endif // ARIADNE_HYBRID_PAVING_HPP
