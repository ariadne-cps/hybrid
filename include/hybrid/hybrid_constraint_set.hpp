/***************************************************************************
 *            system/hybrid_constraint_set.hpp
 *
 *  Copyright  2008-20  Pieter Collins
 *
 ****************************************************************************/

/** @file hybrid/hybrid_constraint_set.hpp
 *  @brief Constraints on hybrid system states.
 */

#ifndef ARIADNE_SYSTEM_HYBRID_CONSTRAINT_SET_HPP
#define ARIADNE_SYSTEM_HYBRID_CONSTRAINT_SET_HPP

#include "hybrid/hybrid_set_interface.hpp"
#include "expression-set/expression_set.hpp"
#include "utility/container.hpp"

namespace Ariadne {

/** @brief HybridBoxesSet represents constraints on hybrid system states. */
class HybridBoxesSet;

//! \ingroup ExpressionSetSubModule
//! \ingroup HybridSetSubModule
//! \brief A hybrid set defined by a constraint system in each location.
//! NOTE: If a location is not specified, the set is considered empty. This may be changed in future to the set being considered unconstrained (entire).
class HybridConstraintSet
    : public virtual EffectiveHybridRegularSetInterface
{
    Map<DiscreteLocation, RealExpressionConstraintSet> _sets;
  public:
    /** @brief Construct or destroy an instance. */
    HybridConstraintSet();
    //! \brief Construct a set in a single \a location with a list of \a bounds on the variables and nonlinear \a constraints.
    HybridConstraintSet(const DiscreteLocation& location,
                        const List<ContinuousPredicate>& constraints);
    HybridConstraintSet(const DiscreteLocation& location,
                        const RealExpressionConstraintSet& constraint_set);
    HybridConstraintSet(const Set<DiscreteLocation>& locations,
                        const RealExpressionConstraintSet& constraint_set);

    //! \brief Adjoin the set defined by \a constraints in \a location.
    HybridConstraintSet& adjoin(const DiscreteLocation& location, const List<ContinuousPredicate>& set);
    /** @brief Perform the hybrid-system operation. */
    HybridConstraintSet& adjoin(const DiscreteLocation& location, const RealExpressionConstraintSet& set);

    //! \brief Intersect with a set of boxes, one in each location, to form a bounded set.
    friend HybridBoundedConstraintSet intersection(const HybridBoxesSet& bounds, HybridConstraintSet const& constraints);

    /** @brief Perform the hybrid-system operation. */
    HybridConstraintSet* clone() const;


    //! \brief The set of locations in which the set is nonempty.
    Set<DiscreteLocation> locations() const;
    /** @brief Return the requested property or representation. */
    Bool has_location(DiscreteLocation loc) const;
    //! \brief The active variables in the location \a loc.
    virtual Set<RealVariable> variables(DiscreteLocation loc) const override;
    //! \brief The subset of \f$\mathbb{R}^V\f$ obtained by restricting to location \a loc.
    RealExpressionConstraintSet const& continuous_set(DiscreteLocation loc) const;
    //! \brief The subset of \f$\mathbb{R}^n\f$ obtained by restricting to location \a loc and ordering the variables as defined by \a spc.
    ConstraintSet const euclidean_set(DiscreteLocation loc, RealSpace spc) const;

    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean overlaps(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean separated(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean covers(const HybridExactBox& bx) const override;

    /** @brief Perform the hybrid-system operation. */
    virtual OutputStream& _write(OutputStream& os) const override;
  protected:
    HybridSetInterfaceBase* _clone() const override { return this->clone(); }
    /** @brief Perform the hybrid-system operation. */
    virtual EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation loc, RealSpace spc) const override;
};

} // namespace Ariadne

#endif // ARIADNE_SYSTEM_HYBRID_CONSTRAINT_SET_HPP
