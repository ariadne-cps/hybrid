/***************************************************************************
 *            hybrid/hybrid_set_interface.hpp
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

/** @file hybrid/hybrid_set_interface.hpp
 *  @brief Interfaces for hybrid-space sets.
 */

#ifndef ARIADNE_HYBRID_SET_INTERFACE_HPP
#define ARIADNE_HYBRID_SET_INTERFACE_HPP

#include <iosfwd>
#include <utility>
#include <map>
#include <memory>
#include <typeinfo>


#include "foundation/logical.hpp"
#include "geometry/set_interface.hpp"
#include "hybrid/discrete_location.hpp"
#include "symbolic/space.hpp"
#include "geometry/set.hpp"
#include "expression-set/expression_set.hpp"
#include "hybrid/hybrid_set.decl.hpp"

namespace Ariadne {

/** @brief HybridSetInterfaceBase represents interfaces for hybrid-space sets. */
class HybridSetInterfaceBase;

template<class P> class HybridBoundedSetInterface;
template<class P> class HybridOpenSetInterface;
template<class P> class HybridClosedSetInterface;
template<class P> class HybridOvertSetInterface;
template<class P> class HybridCompactSetInterface;
template<class P> class HybridRegularSetInterface;
template<class P> class HybridLocatedSetInterface;
template<class P> class HybridRegularLocatedSetInterface;

using EffectiveHybridBoundedSetInterface = HybridBoundedSetInterface<EffectiveTag>;
using EffectiveHybridOpenSetInterface = HybridOpenSetInterface<EffectiveTag>;
using EffectiveHybridClosedSetInterface = HybridClosedSetInterface<EffectiveTag>;
using EffectiveHybridOvertSetInterface = HybridOvertSetInterface<EffectiveTag>;
using EffectiveHybridCompactSetInterface = HybridCompactSetInterface<EffectiveTag>;
using EffectiveHybridRegularSetInterface = HybridRegularSetInterface<EffectiveTag>;
using EffectiveHybridLocatedSetInterface = HybridLocatedSetInterface<EffectiveTag>;
using EffectiveHybridRegularLocatedSetInterface = HybridRegularLocatedSetInterface<EffectiveTag>;

using ValidatedHybridBoundedSetInterface = HybridBoundedSetInterface<ValidatedTag>;
using ValidatedHybridOpenSetInterface = HybridOpenSetInterface<ValidatedTag>;
using ValidatedHybridClosedSetInterface = HybridClosedSetInterface<ValidatedTag>;
using ValidatedHybridOvertSetInterface = HybridOvertSetInterface<ValidatedTag>;
using ValidatedHybridCompactSetInterface = HybridCompactSetInterface<ValidatedTag>;
using ValidatedHybridRegularSetInterface = HybridRegularSetInterface<ValidatedTag>;
using ValidatedHybridLocatedSetInterface = HybridLocatedSetInterface<ValidatedTag>;
using ValidatedHybridRegularLocatedSetInterface = HybridRegularLocatedSetInterface<ValidatedTag>;

template<class P> using HybridSetInterface = HybridRegularLocatedSetInterface<P>;
using EffectiveHybridSetInterface = HybridSetInterface<EffectiveTag>;
using ValidatedHybridSetInterface = HybridSetInterface<ValidatedTag>;

//! \ingroup HybridSetSubmodule
//! \ingroup EuclideanExpressionSet
//! \brief Base class for sets in a hybrid space.
class HybridSetInterfaceBase
{
  public:
    /** @brief Construct or destroy an instance. */
    virtual ~HybridSetInterfaceBase() = default;
    HybridSetInterfaceBase* clone() const { return this->_clone(); }
    /** @brief Return the requested property or representation. */
    virtual Set<RealVariable> variables(DiscreteLocation) const = 0;
    /** @brief Perform the hybrid-system operation. */
    inline EuclideanSetBase euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EuclideanSetBase(this->_euclidean_set(loc,spc)); }
    /** @brief Perform the hybrid-system operation. */
    virtual OutputStream& _write(OutputStream& os) const = 0;
    friend OutputStream& operator<<(OutputStream& os, const HybridSetInterfaceBase& hs);
  protected:
    /** @brief Perform the hybrid-system operation. */
    virtual HybridSetInterfaceBase* _clone() const = 0;
    /** @brief Perform the hybrid-system operation. */
    template<class Interface> Interface* _clone_as() const {
        /** @brief Perform the hybrid-system operation. */
        std::unique_ptr<HybridSetInterfaceBase> result(this->_clone());
        Interface* typed=dynamic_cast<Interface*>(result.get());
        if(typed==nullptr) { throw std::bad_cast(); }
        /** @brief Perform the hybrid-system operation. */
        result.release();
        return typed;
    }
    // One virtual conversion point: typed views use checked cross-casts.
    // In particular, do not introduce covariant _euclidean_set overrides
    // along the multiply virtually inherited hybrid interface hierarchy.
    /** @brief Perform the hybrid-system operation. */
    virtual EuclideanSetInterfaceBase* _euclidean_set(DiscreteLocation,RealSpace) const = 0;

    template<class Interface>
    /** @brief Perform the hybrid-system operation. */
    Interface* _euclidean_set_as(DiscreteLocation loc, RealSpace spc) const {
        /** @brief Perform the hybrid-system operation. */
        std::unique_ptr<EuclideanSetInterfaceBase> result(this->_euclidean_set(loc,spc));
        Interface* typed=dynamic_cast<Interface*>(result.get());
        if(typed==nullptr) { throw std::bad_cast(); }
        /** @brief Perform the hybrid-system operation. */
        result.release();
        return typed;
    }
};

//! \brief Interface for bounded sets in a hybrid space.
template<> class HybridBoundedSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
{
  public:
    HybridBoundedSetInterface<EffectiveTag>* clone() const { return this->_clone_as<HybridBoundedSetInterface<EffectiveTag>>(); }
    /** @brief Return the requested property or representation. */
    virtual Set<DiscreteLocation> locations() const = 0;
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanBoundedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanBoundedSet(this->_euclidean_set_as<EuclideanBoundedSetInterface<EffectiveTag>>(loc,spc)); }
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean inside(const HybridExactBoxes& bx) const = 0;
    /** @brief Perform the hybrid-system operation. */
    virtual HybridUpperBoxes bounding_box() const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for overt sets in a hybrid space.
template<> class HybridOvertSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
{
  public:
    HybridOvertSetInterface* clone() const { return this->_clone_as<HybridOvertSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanOvertSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanOvertSet(this->_euclidean_set_as<EuclideanOvertSetInterface<EffectiveTag>>(loc,spc)); }
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean overlaps(const HybridExactBox& bx) const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for open sets in a hybrid space.
template<> class HybridOpenSetInterface<EffectiveTag>
    : public virtual HybridOvertSetInterface<EffectiveTag>
{
  public:
    HybridOpenSetInterface* clone() const { return this->_clone_as<HybridOpenSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanOpenSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanOpenSet(this->_euclidean_set_as<EuclideanOpenSetInterface<EffectiveTag>>(loc,spc)); }
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean covers(const HybridExactBox& bx) const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for closed sets in a hybrid space.
template<> class HybridClosedSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
{
  public:
    HybridClosedSetInterface* clone() const { return this->_clone_as<HybridClosedSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanClosedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanClosedSet(this->_euclidean_set_as<EuclideanClosedSetInterface<EffectiveTag>>(loc,spc)); }
    /** @brief Perform the hybrid-system operation. */
    virtual LowerKleenean separated(const HybridExactBox& bx) const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for compact (closed and bounded) sets in a hybrid space.
template<> class HybridCompactSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridBoundedSetInterface<EffectiveTag>
    , public virtual HybridClosedSetInterface<EffectiveTag>
{
  public:
    HybridCompactSetInterface* clone() const { return this->_clone_as<HybridCompactSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanCompactSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanCompactSet(this->_euclidean_set_as<EuclideanCompactSetInterface<EffectiveTag>>(loc,spc)); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for regular (open and closed) sets in a hybrid space.
template<> class HybridRegularSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridOpenSetInterface<EffectiveTag>
    , public virtual HybridClosedSetInterface<EffectiveTag>
{
  public:
    HybridRegularSetInterface* clone() const { return this->_clone_as<HybridRegularSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanRegularSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanRegularSet(this->_euclidean_set_as<EuclideanRegularSetInterface<EffectiveTag>>(loc,spc)); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for located (overt and compact) sets in a hybrid space.
template<> class HybridLocatedSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridOvertSetInterface<EffectiveTag>
    , public virtual HybridCompactSetInterface<EffectiveTag>
{
  public:
    HybridLocatedSetInterface* clone() const { return this->_clone_as<HybridLocatedSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanLocatedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanLocatedSet(this->_euclidean_set_as<EuclideanLocatedSetInterface<EffectiveTag>>(loc,spc)); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Complete set interface for bounded regular sets in a hybrid space.
template<> class HybridRegularLocatedSetInterface<EffectiveTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridRegularSetInterface<EffectiveTag>
    , public virtual HybridLocatedSetInterface<EffectiveTag>
{
  public:
    HybridRegularLocatedSetInterface* clone() const { return this->_clone_as<HybridRegularLocatedSetInterface>(); }
    /** @brief Perform the hybrid-system operation. */
    inline EffectiveEuclideanRegularLocatedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const {
        return EffectiveEuclideanRegularLocatedSet(this->_euclidean_set_as<EuclideanRegularLocatedSetInterface<EffectiveTag>>(loc,spc)); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};


inline OutputStream& operator<<(OutputStream& os, const HybridSetInterfaceBase& s) {
    return s._write(os);
}


//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Interface for bounded sets.
template<> class HybridBoundedSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase {
  public:
    HybridBoundedSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridBoundedSetInterface<ValidatedTag>>(); }
    /** @brief Return the requested property or representation. */
    virtual Set<DiscreteLocation> locations() const = 0;
//    inline ValidatedEuclideanBoundedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
    /** @brief Perform the hybrid-system operation. */
    virtual ValidatedLowerKleenean inside(const HybridExactBoxes& bx) const = 0;
    /** @brief Perform the hybrid-system operation. */
    virtual HybridUpperBoxes bounding_box() const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \brief Interface for overt sets in a hybrid space.
template<> class HybridOvertSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase
{
  public:
    HybridOvertSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridOvertSetInterface<ValidatedTag>>(); }
//    inline ValidatedEuclideanOvertSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
    /** @brief Perform the hybrid-system operation. */
    virtual ValidatedLowerKleenean overlaps(const HybridExactBox& bx) const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Interface for open sets.
template<> class HybridOpenSetInterface<ValidatedTag>
    : public virtual HybridOvertSetInterface<ValidatedTag>
{
  public:
    HybridOpenSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridOpenSetInterface<ValidatedTag>>(); }
//    inline EuclideanOpenSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
    /** @brief Perform the hybrid-system operation. */
    virtual ValidatedLowerKleenean covers(const ExactBoxType& bx) const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Interface for closed sets.
template<> class HybridClosedSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase
{
  public:
    HybridClosedSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridClosedSetInterface<ValidatedTag>>(); }
//    inline ValidatedEuclideanClosedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
    /** @brief Perform the hybrid-system operation. */
    virtual ValidatedLowerKleenean separated(const HybridExactBox& bx) const = 0;
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Interface for compact (closed and bounded) sets.
template<> class HybridCompactSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridBoundedSetInterface<ValidatedTag>,
      public virtual HybridClosedSetInterface<ValidatedTag>
{
  public:
    HybridCompactSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridCompactSetInterface<ValidatedTag>>(); }
//    inline ValidatedEuclideanCompactSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Interface for regular sets, whose closure is the closure of the interior, and whose interior is the interior of the closure.
template<> class HybridRegularSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridOpenSetInterface<ValidatedTag>,
      public virtual HybridClosedSetInterface<ValidatedTag>
{
  public:
    HybridRegularSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridRegularSetInterface<ValidatedTag>>(); }
//    inline ValidatedEuclideanRegularSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Interface for located (overt and compact) sets.
template<> class HybridLocatedSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridOvertSetInterface<ValidatedTag>,
      public virtual HybridCompactSetInterface<ValidatedTag>
{
  public:
    HybridLocatedSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridLocatedSetInterface<ValidatedTag>>(); }
//    inline ValidatedEuclideanLocatedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};

//! \ingroup GeometryModule EuclideanSetInterfaceSubModule
//! \brief Complete set interface for bounded regular sets.
template<> class HybridRegularLocatedSetInterface<ValidatedTag>
    : public virtual HybridSetInterfaceBase
    , public virtual HybridRegularSetInterface<ValidatedTag>,
      public virtual HybridLocatedSetInterface<ValidatedTag>
{
  public:
    HybridRegularLocatedSetInterface<ValidatedTag>* clone() const { return this->_clone_as<HybridRegularLocatedSetInterface<ValidatedTag>>(); }
//    inline ValidatedEuclideanRegularLocatedSet euclidean_set(DiscreteLocation loc, RealSpace spc) const { return this->_euclidean_set(loc,spc); }
  protected:
    // The single conversion virtual lives in HybridSetInterfaceBase.
};



} // namespace Ariadne


#endif // ARIADNE_SET_INTERFACE
