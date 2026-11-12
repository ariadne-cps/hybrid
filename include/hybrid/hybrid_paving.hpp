/***************************************************************************
 *            hybrid/hybrid_paving.hpp
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

/** @file hybrid/hybrid_paving.hpp
 *  @brief Pavings of hybrid state spaces.
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
      /** @brief Perform the hybrid-system operation. */
      : _hgrid(grid),_map(),_factory(std::move(factory)) {
        /** @brief Construct or destroy an instance. */
        ARIADNE_PRECONDITION_MSG(static_cast<Bool>(_factory),"Empty HybridPaving factory");
    }
    HybridPaving(HybridGrid const& grid,std::shared_ptr<RobddManager> manager)
      : HybridPaving(grid,[manager=std::move(manager)](Grid const& g) {
        return Paving(new RobddPaving(manager,g));
    }) { }
    HybridPaving* clone() const { return new HybridPaving(*this); }
    // Create another hybrid paving on a different hybrid grid while
    // preserving the selected backend factory and shared ROBDD manager.
    /** @brief Perform the hybrid-system operation. */
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
    /** @brief Perform the hybrid-system operation. */
    Paving const& euclidean_set(DiscreteLocation q,RealSpace const& s) const {
        /** @brief Construct or destroy an instance. */
        ARIADNE_PRECONDITION_MSG(s==space(q),"Wrong variable ordering in "<<q);
        return (*this)[q];
    }
    /** @brief Perform the hybrid-system operation. */
    Void adjoin(HybridGridCell const&);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin(ListSet<HybridGridCell> const&);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin(HybridPaving const&);
    /** @brief Perform the hybrid-system operation. */
    Void remove(HybridPaving const&);
    /** @brief Perform the hybrid-system operation. */
    Void restrict(HybridPaving const&);
    /** @brief Perform the hybrid-system operation. */
    Void clear();
    /** @brief Perform the hybrid-system operation. */
    Void restrict_to_extent(Nat);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_inner_approximation(HybridExactBoxes const&,Nat);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_inner_approximation(EffectiveHybridSetInterface const&,Nat);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_lower_approximation(EffectiveHybridOvertSetInterface const&,Nat,Nat);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_outer_approximation(EffectiveHybridCompactSetInterface const&,Nat);
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_outer_approximation(HybridExactBoxes const&,Nat);
    /** @brief Return the requested property or representation. */
    Bool is_empty() const;
    /** @brief Return the requested property or representation. */
    SizeType size() const;
    /** @brief Perform the hybrid-system operation. */
    HybridListSet<ExactBoxType> boxes() const;
    /** @brief Perform the hybrid-system operation. */
    Void mince(Nat);
    /** @brief Perform the hybrid-system operation. */
    Void recombine();
    /** @brief Perform the hybrid-system operation. */
    ValidatedLowerKleenean separated(HybridExactBox const&) const;
    /** @brief Perform the hybrid-system operation. */
    ValidatedLowerKleenean overlaps(HybridExactBox const&) const;
    /** @brief Perform the hybrid-system operation. */
    ValidatedLowerKleenean covers(HybridExactBox const&) const;
    /** @brief Perform the hybrid-system operation. */
    ValidatedLowerKleenean inside(HybridExactBoxes const&) const;
    /** @brief Perform the hybrid-system operation. */
    HybridUpperBoxes bounding_box() const;
    /** @brief Perform the hybrid-system operation. */
    OutputStream& _write(OutputStream&) const;
    /** @brief Render the object. */
    Void draw(CanvasInterface&,Set<DiscreteLocation> const&,Variables2d const&) const override;
    /** @brief Perform the hybrid-system operation. */
    friend Bool subset(HybridPaving const&,HybridPaving const&);
    /** @brief Perform the hybrid-system operation. */
    friend Bool intersect(HybridPaving const&,HybridPaving const&);
    friend OutputStream& operator<<(OutputStream& os,HybridPaving const& p) {
        return os<<"HybridPaving("<<p._map<<")";
    }
  private:
    /** @brief Perform the hybrid-system operation. */
    Paving& _provide_location(DiscreteLocation const&);
    HybridGrid _hgrid;
    Map<DiscreteLocation,Paving> _map;
    Factory _factory;
};

inline HybridPaving inner_approximation(
        EffectiveHybridSetInterface const& set,HybridGrid const& grid,Nat fineness) {
    /** @brief Perform the hybrid-system operation. */
    HybridPaving result(grid);
    /** @brief Perform the hybrid-system operation. */
    result.adjoin_inner_approximation(set,fineness);
    return result;
}
} // namespace Ariadne
#endif // ARIADNE_HYBRID_PAVING_HPP
