/***************************************************************************
 *            tutorial/tutorial_hybrid.cpp
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

/** @file tutorial/tutorial_hybrid.cpp
 *  @brief Demonstrate tutorial hybrid behavior.
 */

#include <cassert>
#include <iostream>
#include <string>

#include "ariadne-hybrid.hpp"

using namespace Ariadne;
using std::cout;

namespace {

void section(const std::string& title) {
    cout << "\n== " << title << " ==\n";
}

template<class T>
void show(const std::string& label, const T& value) {
    cout << label << ": " << value << "\n";
}

} // namespace

int main() {
    section("Discrete locations");
    // Locations are valuations of discrete variables, not automaton modes.
    DiscreteLocation stationary(0);
    DiscreteLocation moving(1);
    assert(are_distinguishable(stationary,moving));
    show("stationary location",stationary);
    show("moving location",moving);

    section("Continuous state variables in each location");
    // Different locations may have different continuous state spaces.
    RealVariable x("x");
    RealVariable velocity("velocity");
    RealSpace position({x});
    RealSpace position_velocity({x,velocity});

    MonolithicHybridSpace locations;
    locations.new_location(stationary,position);
    locations.new_location(moving,position_velocity);
    HybridSpace space(locations);

    assert(space.has_location(stationary));
    assert(space.has_location(moving));
    assert(space[stationary].dimension()==1u);
    assert(space[moving].dimension()==2u);
    show("hybrid space",space);
    show("dimension in stationary location",space[stationary].dimension());
    show("dimension in moving location",space[moving].dimension());

    section("A hybrid box");
    // Geometry is indexed by location, independent of any dynamics.
    HybridExactBox box(moving,position_velocity,ExactBoxType({{0,1},{-1,1}}));
    show("box location",box.location());
    show("box",box);
    show("box dimension",box.dimension());
    assert(box.location()==moving);
    assert(box.dimension()==2u);

    section("Grids and paving");
    HybridGrid grid(space);
    HybridPaving paving(grid);
    assert(grid.has_location(moving));
    assert(paving.is_empty());
    show("empty paving",paving);

    // Approximate a bounded hybrid set in the moving location.
    // This is set geometry, not a simulation or reachability computation.
    HybridBoundedConstraintSet initial(moving,{0<=x<=1,-1<=velocity<=1});
    paving.adjoin_outer_approximation(initial,4u);
    assert(paving.nontrivial_location(moving));
    assert(paving.size()>0u);
    show("bounded hybrid set",initial);
    show("outer paving",paving);
    show("paving cells",paving.size());

    section("Which layer provides what?");
    cout << "Hybrid: discrete locations, spaces, sets, grids and pavings.\n"
         << "System: dynamical systems and automata (not used here).\n"
         << "Evolution: simulation and enclosure algorithms (not used here).\n"
         << "Analysis: reachability and safety analysis (not used here).\n";
    return 0;
}
