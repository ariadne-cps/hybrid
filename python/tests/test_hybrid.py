"""Smoke tests for genuine hybrid representations in the composed pyariadne_hybrid extension."""

from pyariadne_hybrid import (
    DiscreteLocation,
    HybridRealBox,
    HybridSpace,
    MonolithicHybridSpace,
    Real,
    RealBox,
    RealInterval,
    RealSpace,
    RealVariable,
    are_distinguishable,
    are_same,
)


def test_discrete_locations():
    first = DiscreteLocation(1)
    copy = DiscreteLocation(1)
    second = DiscreteLocation(2)

    assert first == copy
    assert first != second
    assert are_same(first, copy)
    assert are_distinguishable(first, second)
    assert str(first)


def test_space():
    x = RealVariable("x")
    location = DiscreteLocation(1)
    real_space = RealSpace([x])

    monolithic = MonolithicHybridSpace()
    monolithic.new_location(location, real_space)
    assert monolithic.has_location(location)
    assert str(monolithic)

    space = HybridSpace(monolithic)
    assert space.has_location(location)


def test_hybrid_real_box():
    location = DiscreteLocation(1)
    x = RealVariable("x")
    interval = RealInterval(Real(0), Real(1))
    box = HybridRealBox(location, RealSpace([x]), RealBox([interval]))

    assert box.location() == location
    assert box.dimension() == 1
    assert str(box)
