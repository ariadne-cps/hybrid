/***************************************************************************
 *            hybrid/hybrid_bounded_constraint_set.hpp
 *
 *  Copyright  2026  Luca Geretti
 *
 ****************************************************************************/

/** @file hybrid/hybrid_bounded_constraint_set.hpp
 *  @brief Bounded constraints over hybrid states.
 */

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

// Bounded symbolic sets in hybrid state spaces.
#ifndef ARIADNE_SYSTEM_HYBRID_BOUNDED_CONSTRAINT_SET_HPP
#define ARIADNE_SYSTEM_HYBRID_BOUNDED_CONSTRAINT_SET_HPP

#include "hybrid/hybrid_drawable_interface.hpp"
#include "hybrid/hybrid_set_interface.hpp"
#include "utility/container.hpp"

#include "expression-set/expression_set.hpp"

namespace Ariadne {

//! \ingroup ExpressionSetSubModule
//! \ingroup HybridSetSubModule
//! \brief A hybrid set defined by the intersection of a box and a constraint system in each location.
class HybridBoundedConstraintSet
    : public virtual EffectiveHybridSetInterface,
      public virtual HybridDrawableInterface {
    Map<DiscreteLocation, RealExpressionBoundedConstraintSet> _sets;

  public:
    /** @brief Construct or destroy an instance. */
    HybridBoundedConstraintSet();
    HybridBoundedConstraintSet(const DiscreteLocation& loc,
                               const RealVariablesBox& bx);
    HybridBoundedConstraintSet(const DiscreteLocation& loc,
                               const RealExpressionBoundedConstraintSet& set);
    HybridBoundedConstraintSet(const DiscreteLocation& loc,
                               const InitializerList<RealVariableInterval>& bnd);
    //! \brief Construct a set in a single \a location with a list of \a bounds on the variables and nonlinear \a constraints.
    HybridBoundedConstraintSet(const DiscreteLocation& location,
                               const InitializerList<RealVariableInterval>& bounds,
                               const InitializerList<ContinuousPredicate>& constraints);
    HybridBoundedConstraintSet(const Set<DiscreteLocation>& locations,
                               const RealExpressionBoundedConstraintSet& constraint_set);

    /** @brief Perform the hybrid-system operation. */
    HybridBoundedConstraintSet& adjoin(const DiscreteLocation& loc, const RealExpressionBoundedConstraintSet& set);

    /** @brief Perform the hybrid-system operation. */
    HybridBoundedConstraintSet* clone() const;

    //! \brief If the set only has a single location, returns this, otherwise fails. DEPRECATED
    DiscreteLocation location() const;

    //! \brief The set of discrete locations in which the set is nontrivial.
    virtual Set<DiscreteLocation> locations() const override;
    //! \brief The active variables in the location \a loc.
    virtual Set<RealVariable> variables(DiscreteLocation loc) const override;
    //! \brief The subset of \f$\mathbb{R}^V\f$ obtained by restricting to location \a loc.
    const RealExpressionBoundedConstraintSet& continuous_set(DiscreteLocation loc) const;
    //! \brief The subset of \f$\mathbb{R}^n\f$ obtained by restricting to location \a loc and ordering the variables as defined by \a spc.
    const BoundedConstraintSet euclidean_set(DiscreteLocation loc, RealSpace spc) const;

    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean overlaps(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean inside(const HybridExactBoxes& bx) const override;

    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean separated(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean covers(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual HybridUpperBoxes bounding_box() const override;

    /** @brief Perform the hybrid-system operation. */
    virtual OutputStream& _write(OutputStream& os) const override;
    /** @brief Render the object. */
    virtual Void draw(CanvasInterface&, const Set<DiscreteLocation>&, const Variables2d&) const override;

  protected:
    HybridSetInterfaceBase* _clone() const override { return this->clone(); }
    /** @brief Perform the hybrid-system operation. */
    virtual EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation loc, RealSpace spc) const override;
};

} // namespace Ariadne

#endif // ARIADNE_SYSTEM_HYBRID_BOUNDED_CONSTRAINT_SET_HPP
