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
//! A set comprising a %BinaryTreePaving in each location.
class HybridBinaryTreePaving
    : public HybridDrawableInterface
{
  public:
    HybridGrid _hgrid;
    Map<DiscreteLocation,BinaryTreePaving> _map;
  public:
    typedef HybridGrid GridType;
    typedef Map<DiscreteLocation,BinaryTreePaving>::Iterator LocationsIterator;
    typedef Map<DiscreteLocation,BinaryTreePaving>::ConstIterator LocationsConstIterator;
    typedef HybridSpaceSetConstIterator<BinaryTreePaving,HybridGridCell> ConstIterator;
  public:
    //!
    LocationsIterator locations_begin() { return this->_map.begin(); }
    //!
    LocationsIterator locations_end() { return this->_map.end(); }
    //!
    LocationsConstIterator locations_begin() const { return this->_map.begin(); }
    //!
    LocationsConstIterator locations_end() const { return this->_map.end(); }
    //!
    ConstIterator begin() const;
    //!
    ConstIterator end() const;
  public:
    //! Construct from a hybrid grid.
    HybridBinaryTreePaving(const HybridGrid& hgrid) : _hgrid(hgrid), _map() { }

    //! The hybrid grid.
    HybridGrid grid() const { return this->_hgrid; }

    //! Test if \a q is a location of the set i.e. corresponds to a valid location of the underlying hybrid space.
    Bool has_location(DiscreteLocation q) const { return _hgrid.has_location(q); }
    //! Test if \a q is a nontrivial location of the set i.e. contained in the map of <DiscreteLocation,BinaryTreePaving> pairs.
    Bool nontrivial_location(DiscreteLocation q) const { return _map.has_key(q); }
    //! The continuous state space corresponding to location \a q.
    RealSpace space(DiscreteLocation q) const { return _hgrid.space(q); }
    //! The continuous state space corresponding to location \a q.
    const BinaryTreePaving& euclidean_set(DiscreteLocation q) const { return _map[q]; }
    //! The continuous state space corresponding to location \a q.
    const BinaryTreePaving& euclidean_set(DiscreteLocation q, const RealSpace& s) const {
        static_cast<void>(s);
        ARIADNE_PRECONDITION_MSG(s==this->space(q),"Variable ordering in location "<<q<<" is "<<this->space(q)<<", which does not match requested ordering "<<s);
        return _map[q]; }

    //!
//    Void insert(DiscreteLocation q, const BinaryTreePaving& gts) {
//        this->_map.insert(q,gts); }
//    Void insert(Pair<DiscreteLocation,BinaryTreePaving>& qgts) {
//        this->_map.insert(qgts); }

    //!
//    Void adjoin(DiscreteLocation q, const GridCell& c) {
//        this->_provide_location(q).adjoin(c); }

    Void clear();
    Void adjoin(const HybridGridCell& hgc);
    Void adjoin(const ListSet<HybridGridCell>& hgcls);
    Void adjoin(const HybridBinaryTreePaving& hgts);
    Void remove(const HybridBinaryTreePaving& hgts);
    Void restrict(const HybridBinaryTreePaving& hgts);
    Void restrict_to_extent(Nat extent);
    Void adjoin_inner_approximation(const HybridExactBoxes& hbxs, const Nat fineness);
    Void adjoin_inner_approximation(const EffectiveHybridSetInterface& hs, const Nat fineness);
    Void adjoin_lower_approximation(const EffectiveHybridOvertSetInterface& hs, const Nat extent, const Nat fineness);
    Void adjoin_outer_approximation(const EffectiveHybridCompactSetInterface& hs, const Nat fineness);
    Void adjoin_outer_approximation(const HybridExactBoxes& hbxs, const Nat fineness);
    template<class S> Void adjoin_outer_approximation(DiscreteLocation q, const S& s);

    BinaryTreePaving& operator[](DiscreteLocation q);
    const BinaryTreePaving& operator[](DiscreteLocation q) const ;
    Bool is_empty() const;
    SizeType size() const;
    HybridListSet<ExactBoxType> boxes() const;
    Void mince(Nat fineness);
    Void recombine();

    friend Bool subset(const HybridBinaryTreePaving& hgts1, const HybridBinaryTreePaving& hgts2);
    friend Bool intersect(const HybridBinaryTreePaving& hgts1, const HybridBinaryTreePaving& hgts2);
  public:
    //!@{ \name HybridSetInterface methods
    HybridBinaryTreePaving* clone() const { return new HybridBinaryTreePaving(*this); }
    HybridSpace space() const { return this->grid().space(); }
    ValidatedLowerKleenean separated(const HybridExactBox& hbx) const;
    ValidatedLowerKleenean overlaps(const HybridExactBox& hbx) const;
    ValidatedLowerKleenean covers(const HybridExactBox& hbx) const;
    ValidatedLowerKleenean inside(const HybridExactBoxes& hbx) const ;
    HybridUpperBoxes bounding_box() const;
    OutputStream& _write(OutputStream& os) const;
    Void draw(CanvasInterface& c, const Set<DiscreteLocation>& l, const Variables2d&v) const;
    //!@}
  public:
    friend OutputStream& operator<<(OutputStream& os, const HybridBinaryTreePaving& hgts) {
        return os << "HybridBinaryTreePaving(" << hgts._map << ")"; }
  private:
    BinaryTreePaving& _provide_location(const DiscreteLocation& q);
};

//! \brief A location-indexed paving using one selected backend per location.
//! \details A supplied factory selects the backend at first insertion; this
//! choice survives copies and mutations. A shared RobddManager constructor
//! makes all location pavings interoperable in one canonical node table.
class HybridPaving {
  public:
    using Factory = std::function<Paving(Grid const&)>;
    using GridType = HybridGrid;
    using LocationsIterator = Map<DiscreteLocation,Paving>::Iterator;
    using LocationsConstIterator = Map<DiscreteLocation,Paving>::ConstIterator;
    using ConstIterator = HybridSpaceSetConstIterator<Paving,HybridGridCell>;

    explicit HybridPaving(HybridGrid const& grid)
        : HybridPaving(grid,[](Grid const& g) {
            return Paving(new BinaryTreePaving(g));
        }) { }

    HybridPaving(HybridGrid const& grid, Factory factory)
        : _hgrid(grid), _map(), _factory(std::move(factory)) {
        ARIADNE_PRECONDITION_MSG(static_cast<Bool>(_factory),
            "HybridPaving requires a backend factory");
    }

    HybridPaving(HybridGrid const& grid,
                 std::shared_ptr<RobddManager> manager)
        : HybridPaving(grid,[manager=std::move(manager)](Grid const& g) {
            return Paving(new RobddPaving(manager,g));
        }) { }

    HybridGrid grid() const { return _hgrid; }
    HybridSpace space() const { return _hgrid.space(); }
    RealSpace space(DiscreteLocation q) const { return _hgrid.space(q); }
    Bool has_location(DiscreteLocation q) const {
        return _hgrid.has_location(q);
    }
    Bool nontrivial_location(DiscreteLocation q) const {
        return _map.has_key(q);
    }

    LocationsIterator locations_begin() { return _map.begin(); }
    LocationsIterator locations_end() { return _map.end(); }
    LocationsConstIterator locations_begin() const { return _map.begin(); }
    LocationsConstIterator locations_end() const { return _map.end(); }
    ConstIterator begin() const {
        return ConstIterator(_map,_hgrid.space(),false);
    }
    ConstIterator end() const {
        return ConstIterator(_map,_hgrid.space(),true);
    }

    Paving& operator[](DiscreteLocation q) { return _provide_location(q); }
    Paving const& operator[](DiscreteLocation q) const {
        auto i=_map.find(q);
        ARIADNE_PRECONDITION_MSG(i!=_map.end(),
            "No paving has been materialized at the requested location");
        return i->second;
    }
    Paving const& euclidean_set(DiscreteLocation q) const {
        return (*this)[q];
    }

    Void adjoin(HybridGridCell const& cell) {
        _provide_location(cell.location()).adjoin(cell.euclidean_set());
    }
    Void adjoin(HybridPaving const& other) {
        for(auto i=other.locations_begin();i!=other.locations_end();++i) {
            _provide_location(i->first).adjoin(i->second);
        }
    }
    Void remove(HybridPaving const& other) {
        for(auto i=other.locations_begin();i!=other.locations_end();++i) {
            auto self=_map.find(i->first);
            if(self!=_map.end()) self->second.remove(i->second);
        }
    }
    Void restrict(HybridPaving const& other) {
        for(auto i=_map.begin();i!=_map.end();++i) {
            auto source=other._map.find(i->first);
            if(source==other._map.end()) i->second.clear();
            else i->second.restrict(source->second);
        }
    }
    Void clear() {
        for(auto i=_map.begin();i!=_map.end();++i) i->second.clear();
    }
    Bool is_empty() const {
        for(auto i=_map.begin();i!=_map.end();++i)
            if(not i->second.is_empty()) return false;
        return true;
    }
    SizeType size() const {
        SizeType count=0u;
        for(auto i=_map.begin();i!=_map.end();++i) count+=i->second.size();
        return count;
    }
    Void mince(Nat fineness) {
        for(auto i=_map.begin();i!=_map.end();++i)
            i->second.mince(fineness);
    }
    Void recombine() {
        for(auto i=_map.begin();i!=_map.end();++i)
            i->second.recombine();
    }

    friend Bool subset(HybridPaving const& first, HybridPaving const& second) {
        for(auto i=first.locations_begin();i!=first.locations_end();++i) {
            if(i->second.is_empty()) continue;
            auto other=second._map.find(i->first);
            if(other==second._map.end()
               or not i->second.subset(other->second)) return false;
        }
        return true;
    }
    friend Bool intersect(HybridPaving const& first, HybridPaving const& second) {
        for(auto i=first.locations_begin();i!=first.locations_end();++i) {
            if(i->second.is_empty()) continue;
            auto other=second._map.find(i->first);
            if(other!=second._map.end()
               and i->second.intersects(other->second)) return true;
        }
        return false;
    }

  private:
    Paving& _provide_location(DiscreteLocation q) {
        ARIADNE_PRECONDITION_MSG(_hgrid.has_location(q),
            "HybridPaving location must belong to its grid");
        auto found=_map.find(q);
        if(found==_map.end()) {
            Paving selected=_factory(_hgrid[q]);
            ARIADNE_PRECONDITION_MSG(selected.grid()==_hgrid[q],
                "Selected backend uses a grid incompatible with this location");
            _map.insert(std::make_pair(q,std::move(selected)));
            found=_map.find(q);
        }
        return found->second;
    }

    HybridGrid _hgrid;
    Map<DiscreteLocation,Paving> _map;
    Factory _factory;
};

template<class S> Void HybridBinaryTreePaving::adjoin_outer_approximation(DiscreteLocation q, const S& s) {
    this->_provide_location(q).adjoin_outer_approximation(s); }

inline HybridBinaryTreePaving inner_approximation(const EffectiveHybridSetInterface& set, HybridGrid const& grid, const Nat fineness) {
    HybridBinaryTreePaving paving(grid); paving.adjoin_inner_approximation(set,fineness); return paving; }


} // namespace Ariadne

#endif // ARIADNE_HYBRID_PAVING_HPP
