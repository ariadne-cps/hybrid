/***************************************************************************
 *            src/hybrid_bounded_constraint_set.cpp
 *
 *  Copyright  2026  Luca Geretti
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

/** @file src/hybrid_bounded_constraint_set.cpp
 *  @brief Implement bounded hybrid constraints.
 */

// Mathematical operations on bounded symbolic hybrid sets.
#include "hybrid/hybrid_bounded_constraint_set.hpp"

#include "set/function_set.hpp"
#include "utility/stlio.hpp"

namespace Ariadne {

HybridBoundedConstraintSet::HybridBoundedConstraintSet()
    : _sets() {
}

HybridBoundedConstraintSet::HybridBoundedConstraintSet(const DiscreteLocation& loc,
                                                       const RealExpressionBoundedConstraintSet& set) {
    this->adjoin(loc, set);
}

HybridBoundedConstraintSet::HybridBoundedConstraintSet(const Set<DiscreteLocation>& locs,
                                                       const RealExpressionBoundedConstraintSet& set) {
    for (auto loc: locs) {
        this->adjoin(loc, set);
    }
}

HybridBoundedConstraintSet::HybridBoundedConstraintSet(const DiscreteLocation& loc,
                                                       const InitializerList<RealVariableInterval>& bnd) {
    this->adjoin(loc, RealExpressionBoundedConstraintSet(bnd));
}

HybridBoundedConstraintSet::HybridBoundedConstraintSet(const DiscreteLocation& loc,
                                                       const InitializerList<RealVariableInterval>& bnd,
                                                       const InitializerList<ContinuousPredicate>& cnstr) {
    this->adjoin(loc, RealExpressionBoundedConstraintSet(bnd, cnstr));
}

HybridBoundedConstraintSet::HybridBoundedConstraintSet(const DiscreteLocation& loc,
                                                       const RealVariablesBox& bx) {
    this->adjoin(loc, RealExpressionBoundedConstraintSet(bx));
}

HybridBoundedConstraintSet& HybridBoundedConstraintSet::adjoin(const DiscreteLocation& loc,
                                                               const RealExpressionBoundedConstraintSet& set) {
    _sets.insert(loc, set);
    return *this;
}

HybridBoundedConstraintSet* HybridBoundedConstraintSet::clone() const {
    return new HybridBoundedConstraintSet(*this);
}

Set<RealVariable> HybridBoundedConstraintSet::variables(DiscreteLocation loc) const {
    ARIADNE_PRECONDITION(this->_sets.has_key(loc));
    return _sets[loc].variables();
}

const RealExpressionBoundedConstraintSet& HybridBoundedConstraintSet::continuous_set(DiscreteLocation loc) const {
    ARIADNE_PRECONDITION(this->_sets.has_key(loc));
    return this->_sets[loc];
}

const BoundedConstraintSet HybridBoundedConstraintSet::euclidean_set(DiscreteLocation loc, RealSpace spc) const {
    ARIADNE_PRECONDITION(this->_sets.has_key(loc));
    return BoundedConstraintSet(this->_sets[loc].euclidean_set(spc));
}

EuclideanSetInterfaceBase* HybridBoundedConstraintSet::_euclidean_set(DiscreteLocation loc, RealSpace spc) const {
    return new BoundedConstraintSet(this->euclidean_set(loc, spc));
}

DiscreteLocation HybridBoundedConstraintSet::location() const {
    ARIADNE_PRECONDITION(this->_sets.size() == 1);
    return this->_sets.begin()->first;
}

Set<DiscreteLocation> HybridBoundedConstraintSet::locations() const {
    return this->_sets.keys();
}

OutputStream& HybridBoundedConstraintSet::_write(OutputStream& os) const {
    return os << "HybridBoundedConstraintSet( " << this->_sets << " )";
}

} // namespace Ariadne
