/***************************************************************************
 *            hybrid/hybrid_augmented_paving.hpp
 *
 *  Copyright  2009-20  Pieter Collins
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

/** @file hybrid/hybrid_augmented_paving.hpp
 *  @brief Hybrid pavings with auxiliary variables and mappings.
 */

#ifndef ARIADNE_HYBRID_AUGMENTED_PAVING_HPP
#define ARIADNE_HYBRID_AUGMENTED_PAVING_HPP

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "logging/logging.hpp"
#include "numeric/declarations.hpp"
#include "algebra/declarations.hpp"
#include "function/declarations.hpp"
#include "geometry/declarations.hpp"
#include "utility/pointer.hpp"
#include "utility/container.hpp"

#include "hybrid/hybrid_set.decl.hpp"
#include "hybrid/discrete_location.hpp"

#include "geometry/box.hpp"
// HybridAugmentedPaving owns a HybridPaving by value, so the type must be complete.
#include "hybrid/hybrid_paving.hpp"
#include "labelled/labelled_augmented_paving.hpp"
#include "hybrid/hybrid_auxiliary_mapping.hpp"


namespace Ariadne {

template<class X> class Vector;

/** @brief AugmentedPaving represents hybrid pavings with auxiliary variables and mappings. */
class AugmentedPaving;
/** @brief BinaryTreePaving represents hybrid pavings with auxiliary variables and mappings. */
class BinaryTreePaving;
/** @brief HybridAugmentedPaving represents hybrid pavings with auxiliary variables and mappings. */
class HybridAugmentedPaving;

// FIXME: Replace this proxy with a direct mutable view of a labelled augmented paving.
/** @brief HybridAugmentedPavingElementReference represents hybrid pavings with auxiliary variables and mappings. */
class HybridAugmentedPavingElementReference {
    HybridAugmentedPaving* _hstorage; DiscreteLocation _loc;
  public:
    /** @brief Construct or destroy an instance. */
    HybridAugmentedPavingElementReference(HybridAugmentedPaving* hstorage, DiscreteLocation const& loc) : _hstorage(hstorage), _loc(loc) { }
    /** @brief Construct or destroy an instance. */
    inline operator const LabelledAugmentedPaving () const;
    /** @brief Perform the hybrid-system operation. */
    inline Void clear();
};

//! \ingroup HybridSetSubModule
//! \brief A hybrid paving with a location-indexed algebraic auxiliary mapping.
class HybridAugmentedPaving
{
    HybridPaving  _state_set;
    HybridAuxiliaryMapping _auxiliary_mapping;
  public:
    typedef HybridGrid GridType;
    typedef HybridPaving PavingType;

    HybridAugmentedPaving(HybridGrid const& hg, HybridAuxiliaryMapping const& aux)
        : _state_set(hg), _auxiliary_mapping(aux) { }
    HybridAugmentedPaving(HybridPaving const& hgtp, HybridAuxiliaryMapping const& aux)
        : _state_set(hgtp), _auxiliary_mapping(aux) { }

    HybridAuxiliaryMapping const& auxiliary_data() const { return _auxiliary_mapping; }
    /** @brief Perform the hybrid-system operation. */
    LabelledMapping auxiliary_mapping(DiscreteLocation const& loc) const {
        return _auxiliary_mapping(loc); }

    HybridGrid grid() const { return this->_state_set.grid(); }
    HybridPaving& state_paving() { return this->_state_set; }
    HybridPaving const& state_paving() const { return this->_state_set; }
    //! Compatibility aliases for analysis clients.
    HybridPaving& state_set() { return this->state_paving(); }
    HybridPaving const& state_set() const { return this->state_paving(); }
    /** @brief Perform the hybrid-system operation. */
    HybridPaving state_auxiliary_paving() const;

    Bool is_empty() const { return this->_state_set.is_empty(); }
    SizeType size() const { return this->_state_set.size(); }
    HybridPaving::ConstIterator begin() const { return this->_state_set.begin(); }
    HybridPaving::ConstIterator end() const { return this->_state_set.end(); }

    Void clear() { this->_state_set.clear(); }
    Void adjoin(HybridAugmentedPaving const& other) { return this->_state_set.adjoin(other._state_set); }
    Void remove(HybridAugmentedPaving const& other) { return this->_state_set.remove(other._state_set); }
    Void restrict(HybridAugmentedPaving const& other) { return this->_state_set.restrict(other._state_set); }

    /** @brief Perform the hybrid-system operation. */
    friend Bool subset(HybridAugmentedPaving const& set1, HybridAugmentedPaving const& set2) {
        return subset(set1._state_set,set2._state_set); }
    /** @brief Perform the hybrid-system operation. */
    friend Bool intersect(HybridAugmentedPaving const& set1, HybridAugmentedPaving const& set2) {
        return intersect(set1._state_set,set2._state_set); }

    /** @brief Perform the hybrid-system operation. */
    Void adjoin_lower_approximation(EffectiveHybridOvertSetInterface const& set, Nat extent, Nat fineness) {
        this->_state_set.adjoin_lower_approximation(set,extent,fineness); }
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_outer_approximation(EffectiveHybridCompactSetInterface const& set, Nat fineness) {
        this->_state_set.adjoin_outer_approximation(set,fineness); }
    /** @brief Perform the hybrid-system operation. */
    Void adjoin_outer_approximation(HybridExactBoxes const& bx, Nat fineness) {
        this->_state_set.adjoin_outer_approximation(bx,fineness); }

    Void restrict_to_extent(Nat level) { this->_state_set.restrict_to_extent(level); }
    Void mince(Nat fineness) { this->_state_set.mince(fineness); }
    Void recombine() { this->_state_set.recombine(); }

    const LabelledAugmentedPaving operator[] (DiscreteLocation const& loc) const;
    HybridAugmentedPavingElementReference operator[] (DiscreteLocation const& loc) {
        return HybridAugmentedPavingElementReference{this,loc}; }


    friend OutputStream& operator<<(OutputStream& os, HybridAugmentedPaving const& set) {
        return os << "HybridAugmentedPaving(" << set._state_set << ", state_auxiliary_space="
                  << set._auxiliary_mapping.state_auxiliary_space() << ")"; }

    friend class HybridAugmentedPavingElementReference;
};

inline const LabelledAugmentedPaving HybridAugmentedPaving::operator[](DiscreteLocation const& q) const {
    return LabelledAugmentedPaving(this->_state_set[q],this->auxiliary_mapping(q));
}
/** @brief Construct or destroy an instance. */
HybridAugmentedPavingElementReference::operator const LabelledAugmentedPaving () const {
    return LabelledAugmentedPaving(_hstorage->_state_set[_loc],_hstorage->auxiliary_mapping(_loc)); }
Void HybridAugmentedPavingElementReference::clear() { return _hstorage->state_paving()[_loc].clear(); }

} // namespace Ariadne

#endif // ARIADNE_HYBRID_AUGMENTED_PAVING_HPP
