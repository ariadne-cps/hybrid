/***************************************************************************
 *            hybrid/hybrid_validated_constrained_image_set.hpp
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

/** @file hybrid/hybrid_validated_constrained_image_set.hpp
 *  @brief Concrete validated constrained-image hybrid sets.
 */

#ifndef ARIADNE_HYBRID_VALIDATED_CONSTRAINED_IMAGE_SET_HPP
#define ARIADNE_HYBRID_VALIDATED_CONSTRAINED_IMAGE_SET_HPP

#include "hybrid/hybrid_set.hpp"

#include "expression-set/expression_set.hpp"

namespace Ariadne {

//! \ingroup LabelledSetSubModule
//! \ingroup HybridSetSubModule
//! \brief A hybrid set defined by the intersection of a box and a constraint system in each location.
class HybridValidatedConstrainedImageSet
    : public virtual ValidatedHybridLocatedSetInterface,
      public virtual HybridDrawableInterface,
      public HybridBasicSet<ValidatedConstrainedImageSet> {
    typedef HybridBasicSet<ValidatedConstrainedImageSet> Base;

  public:
    using HybridBasicSet<ValidatedConstrainedImageSet>::HybridBasicSet;

    /** @brief Perform the hybrid-system operation. */
    HybridValidatedConstrainedImageSet* clone() const {
        return new HybridValidatedConstrainedImageSet(*this);
    }

    /** @brief Return the requested property or representation. */
    virtual Set<RealVariable> variables(DiscreteLocation loc) const override {
        static_cast<void>(loc);
        ARIADNE_PRECONDITION(loc == this->location());
        return this->Base::variables();
    }
    /** @brief Return the requested property or representation. */
    virtual Set<DiscreteLocation> locations() const override {
        return {this->Base::location()};
    }

    /** @brief Perform the hybrid-system operation. */
    virtual ValidatedLowerKleenean overlaps(const HybridExactBoxType& hbx) const override {
        return this->Base::overlaps(hbx);
    }
    /** @brief Perform the hybrid-system operation. */
    inline ValidatedLowerKleenean inside(const HybridExactBoxesType& hbxs) const override {
        return this->Base::inside(hbxs);
    }
    /** @brief Perform the hybrid-system operation. */
    virtual ValidatedLowerKleenean separated(const HybridExactBoxType& hbx) const override {
        return this->Base::separated(hbx);
    }
    /** @brief Perform the hybrid-system operation. */
    virtual HybridUpperBoxes bounding_box() const override;

    /** @brief Perform the hybrid-system operation. */
    virtual OutputStream& _write(OutputStream& os) const override {
        return os << static_cast<const Base&>(*this);
    }
    /** @brief Render the object. */
    virtual Void draw(CanvasInterface& cnvs, const Set<DiscreteLocation>& locs, const Variables2d& vars) const override {
        return this->Base::draw(cnvs, locs, vars);
    }

    friend OutputStream& operator<<(OutputStream& os, const HybridValidatedConstrainedImageSet& hset) { return os << static_cast<const Base&>(hset); }

  protected:
    HybridSetInterfaceBase* _clone() const override { return this->clone(); }
    /** @brief Perform the hybrid-system operation. */
    virtual EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation loc, RealSpace spc) const override {
        static_cast<void>(loc);
        static_cast<void>(spc);
        /** @brief Construct or destroy an instance. */
        ARIADNE_PRECONDITION(loc == this->location());
        /** @brief Construct or destroy an instance. */
        ARIADNE_PRECONDITION(spc == this->space());
        return new ValidatedConstrainedImageSet(this->Base::euclidean_set());
    }
};

} // namespace Ariadne

#endif // ARIADNE_HYBRID_VALIDATED_CONSTRAINED_IMAGE_SET_HPP
