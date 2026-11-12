/***************************************************************************
 *            system/hybrid_constraint_set.cpp
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Hybrid. If not, see <https://www.gnu.org/licenses/>.
 */

/** @file src/hybrid_constraint_set.cpp
 *  @brief Implement hybrid constraint sets.
 */

#include "hybrid/hybrid_constraint_set.hpp"

#include "function/functional.hpp"
#include "set/function_set.hpp"
#include "utility/stlio.hpp"

namespace Ariadne {

HybridConstraintSet::HybridConstraintSet()
    : _sets() {
}

HybridConstraintSet::HybridConstraintSet(const DiscreteLocation& loc,
                                         const List<ContinuousPredicate>& cnstr) {
    this->adjoin(loc, RealExpressionConstraintSet(cnstr));
}

HybridConstraintSet::HybridConstraintSet(const DiscreteLocation& loc,
                                         const RealExpressionConstraintSet& cs) {
    this->adjoin(loc, cs);
}

HybridConstraintSet::HybridConstraintSet(const Set<DiscreteLocation>& locs,
                                         const RealExpressionConstraintSet& cs) {
    for (auto loc: locs) {
        this->adjoin(loc, cs);
    }
}

HybridConstraintSet* HybridConstraintSet::clone() const {
    return new HybridConstraintSet(*this);
}

HybridConstraintSet& HybridConstraintSet::adjoin(const DiscreteLocation& loc, const RealExpressionConstraintSet& cs) {
    this->_sets.insert(loc, cs);
    return *this;
}

HybridConstraintSet& HybridConstraintSet::adjoin(const DiscreteLocation& loc, const List<ContinuousPredicate>& cnstr) {
    return this->adjoin(loc, RealExpressionConstraintSet(cnstr));
}

Set<DiscreteLocation> HybridConstraintSet::locations() const {
    return this->_sets.keys();
}

Bool HybridConstraintSet::has_location(DiscreteLocation loc) const {
    return this->_sets.has_key(loc);
}

Set<RealVariable> HybridConstraintSet::variables(DiscreteLocation loc) const {
    ARIADNE_PRECONDITION(this->_sets.has_key(loc));
    return _sets[loc].variables();
}

const RealExpressionConstraintSet& HybridConstraintSet::continuous_set(DiscreteLocation loc) const {
    ARIADNE_PRECONDITION(this->_sets.has_key(loc));
    return this->_sets[loc];
}

const ConstraintSet HybridConstraintSet::euclidean_set(DiscreteLocation loc, RealSpace spc) const {
    ARIADNE_PRECONDITION(this->_sets.has_key(loc));
    return ConstraintSet(this->_sets[loc].euclidean_set(spc));
}

EuclideanSetInterfaceBase* HybridConstraintSet::_euclidean_set(DiscreteLocation loc, RealSpace spc) const {
    return new ConstraintSet(this->euclidean_set(loc, spc));
}

OutputStream& HybridConstraintSet::_write(OutputStream& os) const {
    return os << "HybridConstraintSet( " << this->_sets << " )";
}

} // namespace Ariadne
