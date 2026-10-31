# Ariadne Hybrid

Ariadne Hybrid is the standalone C++20 hybrid-space representation layer.
It defines discrete locations, hybrid spaces, mathematical hybrid sets and
constraints, hybrid grids and pavings, and their drawing interfaces/adapters.

It deliberately does **not** define dynamical systems or hybrid automata,
simulation and evolution algorithms, orbit/enclosure trajectories, or
reachability analysis. Those are owned by the downstream repositories
`system`, `evolution`, and `analysis` respectively.

## Layout

- `include/hybrid/`: public headers.
- `include/ariadne-hybrid.hpp`: umbrella header.
- `src/`: C++ implementations.
- `ss/`: pinned Git submodules.
- `tests/`: standalone C++ representation tests.
- `.github/workflows/`: standalone CI.

The public target is `hybrid`; the standalone library is `ariadne-hybrid`.

## Dependencies

Direct dependencies: [labelled](https://github.com/ariadne-cps/labelled) for
named sets, drawing and expressions; and
[configuration](https://github.com/ariadne-cps/configuration) for shared
build infrastructure. All other dependencies are provided transitively.

## Build and test

```sh
git clone https://github.com/ariadne-cps/hybrid.git
cd hybrid
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel --target everything
ctest --test-dir build --output-on-failure
```

CMake initializes the necessary pinned submodules. The source checkout can
also be initialized with `git submodule update --init` (recursive checkout
is not required by the Ariadne build machinery).

The graphical smoke source `tests/test_hybrid_graphics.cpp` is retained but
not automatically run, matching its previous test registration.

## License

GPL-3.0-or-later; see LICENSE.
