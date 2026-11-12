/***************************************************************************
 *            hybrid/hybrid_expression_set.hpp
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

/** @file hybrid/hybrid_expression_set.hpp
 *  @brief Symbolic expression-defined sets in hybrid spaces.
 */

#ifndef ARIADNE_HYBRID_EXPRESSION_SET_HPP
#define ARIADNE_HYBRID_EXPRESSION_SET_HPP

#include <map>
#include <memory>

#include "algebra/declarations.hpp"
#include "function/declarations.hpp"
#include "geometry/box.hpp"
#include "geometry/declarations.hpp"
#include "geometry/point.hpp"
#include "hybrid/hybrid_bounded_constraint_set.hpp"
#include "hybrid/hybrid_constraint_set.hpp"
#include "hybrid/hybrid_graphics_interface.hpp"
#include "hybrid/hybrid_grid.hpp"
#include "hybrid/hybrid_set.decl.hpp"
#include "hybrid/hybrid_set_interface.hpp"
#include "hybrid/hybrid_space.hpp"
#include "numeric/declarations.hpp"
#include "paving/binary_tree_paving.hpp"
#include "set/curve.hpp"
#include "set/function_set.hpp"
#include "set/list_set.hpp"
#include "utility/container.hpp"
#include "utility/macros.hpp"
#include "utility/stlio.hpp"

#include "expression-set/expression_set.hpp"

namespace Ariadne {

//! \ingroup ExpressionSetSubModule
//! \ingroup HybridSetSubModule
//! \brief A hybrid set defined by a box in a single location.
//! \details Does not assume a canonical order of the real variables.
template<class IVL>
class HybridVariablesBox
    : public Pair<DiscreteLocation, VariablesBox<IVL>> {
    typedef Pair<DiscreteLocation, VariablesBox<IVL>> Base;

  public:
    HybridVariablesBox(const DiscreteLocation& loc, const VariablesBox<IVL>& bx)
        : Base(loc, bx) {}
    HybridVariablesBox(const DiscreteLocation& loc, const RealSpace& spc, const Box<IVL>& bx)
        : Base(loc, VariablesBox<IVL>(spc, bx)) {}

    //! \brief The location in which the box is defined.
    DiscreteLocation location() const { return this->Base::first; }
    //! \brief The location in which the box is defined.
    const VariablesBox<IVL>& continuous_set() const { return this->Base::second; }
    //! \brief The active variables in the location \a loc.
    Set<RealVariable> variables() const { return this->Base::second.variables(); }
    //! \brief The subset of \f$\mathbb{R}^n\f$ obtained by ordering the variables as defined by \a spc.
    Box<IVL> euclidean_set(const RealSpace& spc) const { return this->second.euclidean_set(spc); }
};

/** @brief HybridBoxSet represents symbolic expression-defined sets in hybrid spaces. */
class HybridBoxSet
    : public virtual EffectiveHybridSetInterface,
      public virtual HybridDrawableInterface,
      public HybridVariablesBox<RealInterval> {
  public:
    using HybridVariablesBox<RealInterval>::HybridVariablesBox;
    using HybridVariablesBox<RealInterval>::variables;
    using HybridVariablesBox<RealInterval>::euclidean_set;

    /** @brief Perform the hybrid-system operation. */
    RealBox euclidean_set(DiscreteLocation, RealSpace) const;
    /** @brief Return the requested property or representation. */
    virtual Set<DiscreteLocation> locations() const override;
    /** @brief Return the requested property or representation. */
    virtual Set<RealVariable> variables(DiscreteLocation) const override;
    /** @brief Return the requested property or representation. */
    RealSpace space() const;
    /** @brief Return the requested property or representation. */
    virtual RealSpace space(DiscreteLocation) const;
    /** @brief Return the requested property or representation. */
    virtual LowerKleenean is_empty() const;

    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean overlaps(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean separated(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean covers(const HybridExactBox& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean inside(const HybridExactBox& bx) const;
    //    virtual HybridUpperBox bounding_box() const;

    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean inside(const HybridExactBoxes& bx) const override;
    /** @brief Perform the hybrid-system operation. */
    virtual HybridUpperBoxes bounding_box() const override;

    /** @brief Perform the hybrid-system operation. */
    virtual OutputStream& _write(OutputStream& os) const override;
    /** @brief Render the object. */
    virtual Void draw(CanvasInterface&, const Set<DiscreteLocation>&, const Variables2d&) const override;

  private:
    // Return the exact interface pointer type to avoid covariant-return
    // adjustment across the virtual-inheritance hierarchy on MSVC.
    /** @brief Perform the hybrid-system operation. */
    virtual HybridSetInterfaceBase* _clone() const override;
    /** @brief Perform the hybrid-system operation. */
    virtual EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation, RealSpace) const override;
};

//! \ingroup ExpressionSetSubModule
//! \ingroup HybridSetSubModule
//! \brief A hybrid set defined by a box in each of a set of locations.
//! \details Does not assume a canonical order of the real variables.
template<class IVL>
class HybridVariablesBoxes
    : public Map<DiscreteLocation, VariablesBox<IVL>> {
    typedef Map<DiscreteLocation, VariablesBox<IVL>> Base;

  public:
    /** @brief Construct or destroy an instance. */
    HybridVariablesBoxes() : Base() {}
    /** @brief Construct or destroy an instance. */
    HybridVariablesBoxes(Set<DiscreteLocation> locs, const VariablesBox<IVL>& box) {
        for (auto loc: locs) {
            this->Base::insert(loc, box);
        }
    }
    /** @brief Construct or destroy an instance. */
    HybridVariablesBoxes(const List<HybridVariablesBox<IVL>>& hboxes) {
        for (auto hbox: hboxes) {
            this->adjoin(hbox);
        }
    }

    /** @brief Perform the hybrid-system operation. */
    HybridVariablesBoxes<IVL>& adjoin(const HybridVariablesBox<IVL>& hbox) {
        this->Base::insert(hbox.location(), hbox.continuous_set());
        return *this;
    }

    //! \brief The location in which the box is defined.
    Set<DiscreteLocation> locations() const { return this->Base::keys(); }
    //! \brief The location in which the box is defined.
    const VariablesBox<IVL>& continuous_set(DiscreteLocation loc) const { return (*this)[loc]; }
    //! \brief The active variables in the location \a loc.
    Set<RealVariable> variables(DiscreteLocation loc) const { return this->continuous_set(loc).variables(); }
    //! \brief The subset of \f$\mathbb{R}^n\f$ obtained by ordering the variables as defined by \a spc.
    Box<IVL> euclidean_set(DiscreteLocation loc, const RealSpace& spc) const { return this->continuous_set(loc).euclidean_set(spc); }
};

/** @brief HybridBoxesSet represents symbolic expression-defined sets in hybrid spaces. */
class HybridBoxesSet
    // TODO: Support
    : public HybridVariablesBoxes<RealInterval> {
  public:
    using HybridVariablesBoxes<RealInterval>::HybridVariablesBoxes;
};

} // namespace Ariadne

#endif // ARIADNE_HYBRID_EXPRESSION_SET_HPP
