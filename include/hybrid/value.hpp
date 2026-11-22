/***************************************************************************
 *            hybrid/value.hpp
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

/** @file hybrid/value.hpp
 *  @brief Representation-independent located values and hybrid families.
 */

#ifndef ARIADNE_HYBRID_VALUE_HPP
#define ARIADNE_HYBRID_VALUE_HPP

#include <functional>
#include <optional>
#include <utility>

#include "hybrid/hybrid_space.hpp"
#include "utility/container.hpp"
#include "utility/macros.hpp"

namespace Ariadne {

// A value belonging to exactly one discrete location.
template<class T>
class Located {
    DiscreteLocation _location;
    T _value;

  public:
    using ValueType = T;

    Located(DiscreteLocation location, T value)
        : _location(std::move(location)), _value(std::move(value)) {}

    const DiscreteLocation& location() const { return _location; }
    const T& value() const { return _value; }
    T& value() { return _value; }
};

// A family of values indexed by locations. A hybrid space is optional:
// without it, the family is an explicit map. With it, a factory can
// materialize a value lazily for any valid location. The two sets of
// locations (valid and materialized) must not be confused.
template<class T>
class Hybrid {
  public:
    using ValueType = T;
    using Factory = std::function<T(const DiscreteLocation&, const RealSpace&)>;
    using ConstIterator = typename Map<DiscreteLocation, T>::ConstIterator;
    using Iterator = typename Map<DiscreteLocation, T>::Iterator;

  private:
    Map<DiscreteLocation, T> _values;
    std::optional<HybridSpace> _space;
    Factory _factory;

  public:
    Hybrid() = default;

    Hybrid(HybridSpace space, Factory factory)
        : _values(), _space(std::move(space)), _factory(std::move(factory)) {
        ARIADNE_PRECONDITION(static_cast<bool>(_factory));
    }

    Bool has_location(const DiscreteLocation& location) const {
        return _space ? _space->has_location(location) : _values.has_key(location);
    }

    Bool has_value(const DiscreteLocation& location) const {
        return _values.has_key(location);
    }

    const HybridSpace& space() const {
        ARIADNE_PRECONDITION(_space.has_value());
        return *_space;
    }

    Void insert(const DiscreteLocation& location, T value) {
        ARIADNE_PRECONDITION(!_space || _space->has_location(location));
        ARIADNE_PRECONDITION(!_values.has_key(location));
        _values.insert(location, std::move(value));
    }

    T& operator[](const DiscreteLocation& location) {
        ARIADNE_PRECONDITION(has_location(location));
        auto found = _values.find(location);
        if (found == _values.end()) {
            ARIADNE_PRECONDITION(_space.has_value());
            T value = _factory(location, (*_space)[location]);
            _values.insert(location, std::move(value));
            found = _values.find(location);
        }
        return found->second;
    }

    const T& operator[](const DiscreteLocation& location) const {
        // Const access must not change materialization state.
        ARIADNE_PRECONDITION(has_value(location));
        return _values.find(location)->second;
    }

    Set<DiscreteLocation> materialized_locations() const {
        return _values.keys();
    }

    SizeType size() const { return _values.size(); }
    Iterator begin() { return _values.begin(); }
    Iterator end() { return _values.end(); }
    ConstIterator begin() const { return _values.begin(); }
    ConstIterator end() const { return _values.end(); }
};

// Select a located continuous value, with explicit variable ordering.
// Location is metadata; no rendering or paving knowledge belongs here.
template<class T, class Visitor>
Void with_location(const Located<T>& item, const RealSpace& space, Visitor&& visitor) {
    visitor(space, item.value());
}

// Select an already-materialised member without triggering lazy construction.
template<class T, class Visitor>
Void with_location(const Hybrid<T>& family, const DiscreteLocation& location,
                   const RealSpace& space, Visitor&& visitor) {
    ARIADNE_PRECONDITION(family.has_value(location));
    visitor(space, family[location]);
}

// Infer the continuous variable ordering from the declared hybrid space.
template<class T, class Visitor>
Void with_location(const Hybrid<T>& family, const DiscreteLocation& location,
                   Visitor&& visitor) {
    ARIADNE_PRECONDITION(family.has_value(location));
    const RealSpace space = family.space()[location];
    with_location(family, location, space, std::forward<Visitor>(visitor));
}

} // namespace Ariadne

#endif // ARIADNE_HYBRID_VALUE_HPP
