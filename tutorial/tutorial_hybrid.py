#!/usr/bin/env python3
"""Tour of hybrid *representations*, without automata or dynamics."""

from pyariadne_hybrid import (
    DiscreteLocation,
    HybridGrid,
    HybridBinaryTreePaving,
    HybridRealBox,
    HybridSpace,
    MonolithicHybridSpace,
    Real,
    RealBox,
    RealInterval,
    RealSpace,
    RealVariable,
    are_distinguishable,
)


def section(title):
    print(f"\n== {title} ==")


def show(label, value):
    print(f"{label}: {value}")


def tutorial_hybrid():
    section("Discrete locations")
    stationary = DiscreteLocation(0)
    moving = DiscreteLocation(1)
    assert are_distinguishable(stationary, moving)
    show("stationary location", stationary)
    show("moving location", moving)

    section("Continuous state variables in each location")
    x = RealVariable("x")
    velocity = RealVariable("velocity")
    position = RealSpace([x])
    position_velocity = RealSpace([x, velocity])

    locations = MonolithicHybridSpace()
    locations.new_location(stationary, position)
    locations.new_location(moving, position_velocity)
    space = HybridSpace(locations)

    assert space.has_location(stationary)
    assert space.has_location(moving)
    assert space.space(stationary).dimension() == 1
    assert space.space(moving).dimension() == 2
    show("hybrid space", space)
    show("dimension in stationary location", space.space(stationary).dimension())
    show("dimension in moving location", space.space(moving).dimension())

    section("A hybrid box")
    # Each continuous box belongs to one discrete location.
    real_box = RealBox([
        RealInterval(Real(0), Real(1)),
        RealInterval(Real(-1), Real(1)),
    ])
    box = HybridRealBox(moving, position_velocity, real_box)
    assert box.location() == moving
    assert box.dimension() == 2
    show("box location", box.location())
    show("box", box)
    show("box dimension", box.dimension())

    section("Grids and paving")
    grid = HybridGrid(space)
    paving = HybridBinaryTreePaving(grid)
    assert grid.has_location(moving)
    assert paving.has_location(moving)
    assert paving.is_empty()
    show("grid", grid)
    show("empty paving", paving)

    section("Which layer provides what?")
    print("Hybrid: discrete locations, spaces, sets, grids and pavings.")
    print("System: dynamical systems and automata (not used here).")
    print("Evolution: simulation and enclosure algorithms (not used here).")
    print("Analysis: reachability and safety analysis (not used here).")


if __name__ == "__main__":
    tutorial_hybrid()
