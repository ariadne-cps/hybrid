/***************************************************************************
 *            hybrid/hybrid_scalings.hpp
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

/** @file hybrid/hybrid_scalings.hpp
 *  @brief Scaling of continuous variables in hybrid locations.
 */

#ifndef ARIADNE_HYBRID_SCALINGS_HPP
#define ARIADNE_HYBRID_SCALINGS_HPP

#include <iostream>
#include <map>
#include <memory>

#include "utility/container.hpp"
#include "utility/stlio.hpp"

#include "symbolic/variable.hpp"
#include "symbolic/space.hpp"
#include "hybrid/discrete_location.hpp"
#include "paving/grid.hpp"

namespace Ariadne {

typedef void Void;
typedef std::ostream OutputStream;


//! \ingroup HybridModule
//! \brief A class which defines the state space grid to use in location \a loc given the continuous state variables \a spc.
class HybridScalingsInterface
{
  public:
    //!
    virtual ~HybridScalingsInterface() = default;
    /** @brief Perform the hybrid-system operation. */
    virtual HybridScalingsInterface* clone() const = 0;
    /** @brief Perform the hybrid-system operation. */
    virtual ExactDouble scaling(const DiscreteLocation& loc, const RealVariable& var) const = 0;
    /** @brief Perform the hybrid-system operation. */
    virtual Void _write(OutputStream& os) const = 0;
};
inline OutputStream& operator<<(OutputStream& os, const HybridScalingsInterface& hsc) { hsc._write(os); return os; }


//! \ingroup HybridModule
//! \brief A class which defines the state space grid to use in location \a loc given the continuous state variables \a spc.
class HybridScalings
{
    std::shared_ptr<HybridScalingsInterface> _ptr;
  public:
    /** @brief Construct or destroy an instance. */
    HybridScalings(const HybridScalingsInterface& ref) : _ptr(ref.clone()) { }
    operator const HybridScalingsInterface& () const { return *this->_ptr; }
    operator HybridScalingsInterface& () { return *this->_ptr; }
    ExactDouble scaling(const DiscreteLocation& loc, const RealVariable& var) const { return this->_ptr->scaling(loc,var); }
    /** @brief Return the requested property or representation. */
    Grid grid(const DiscreteLocation& loc, const RealSpace& spc) const;
  public:
    /** @brief Construct or destroy an instance. */
    HybridScalings(const Map<Identifier,ExactDouble>& scalings);
    /** @brief Construct or destroy an instance. */
    HybridScalings(const InitializerList<Pair<RealVariable,ApproximateDouble>>& scalings);
};

/** @brief SimpleHybridScalings represents scaling of continuous variables in hybrid locations. */
class SimpleHybridScalings
    : public HybridScalingsInterface
{
    ExactDouble _default_scaling;
    Map<Identifier,ExactDouble> _scalings;
  public:
    /** @brief Construct or destroy an instance. */
    SimpleHybridScalings() : _default_scaling(1.0_x), _scalings() { }
    SimpleHybridScalings(ExactDouble default_scaling, const Map<Identifier,ExactDouble>& scalings)
        : _default_scaling(default_scaling), _scalings(scalings) { }
    /** @brief Construct or destroy an instance. */
    SimpleHybridScalings(const InitializerList<Pair<RealVariable,ApproximateDouble>>& scalings);
    /** @brief Return the requested property or representation. */
    Void set_scaling(const RealVariable& var, ApproximateDouble scal) {
        ARIADNE_PRECONDITION(decide(scal>0)); _scalings[var.name()]=cast_exact(scal); }
    virtual SimpleHybridScalings* clone() const { return new SimpleHybridScalings(*this); }
    /** @brief Perform the hybrid-system operation. */
    virtual ExactDouble scaling(const DiscreteLocation&, const RealVariable& var) const {
        return (this->_scalings.has_key(var.name())) ? this->_scalings[var.name()] : this->_default_scaling; }
    virtual Void _write(OutputStream& os) const { os << "HybridScalings( " << this->_scalings << " )"; }
};

inline Pair<RealVariable,ExactDouble> operator|(const RealVariable& var, ApproximateDouble scal) {
    return Pair<RealVariable,ExactDouble>(var,cast_exact(scal)); }

inline HybridScalings::HybridScalings(const Map<Identifier,ExactDouble>& scalings)
    : _ptr(new SimpleHybridScalings(1.0_x,scalings)) { }

inline HybridScalings::HybridScalings(const InitializerList<Pair<RealVariable,ApproximateDouble>>& scalings)
    : _ptr(new SimpleHybridScalings(scalings)) { }

inline SimpleHybridScalings::SimpleHybridScalings(const InitializerList<Pair<RealVariable,ApproximateDouble>>& scalings)
    /** @brief Perform the hybrid-system operation. */
    : _default_scaling(1.0_x), _scalings() {
    // Unlisted variables retain the same unit scaling as the default policy.
    for(auto iter=scalings.begin(); iter!=scalings.end(); ++iter) {
        /** @brief Perform the hybrid-system operation. */
        this->_scalings.insert(iter->first.name(),cast_exact(iter->second));
    }
}

}

#endif // ARIADNE_HYBRID_SCALINGS_HPP

