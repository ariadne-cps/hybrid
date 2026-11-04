# Ariadne Hybrid

[![macOS Status](https://github.com/ariadne-cps/hybrid/actions/workflows/macos.yml/badge.svg)](https://github.com/ariadne-cps/hybrid/actions/workflows/macos.yml)
[![Ubuntu Status](https://github.com/ariadne-cps/hybrid/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ariadne-cps/hybrid/actions/workflows/ubuntu.yml)
[![Windows Status](https://github.com/ariadne-cps/hybrid/actions/workflows/win.yml/badge.svg)](https://github.com/ariadne-cps/hybrid/actions/workflows/win.yml)
[![Sanitizers Status](https://github.com/ariadne-cps/hybrid/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/ariadne-cps/hybrid/actions/workflows/sanitizers.yml)
[![Coverage Status](https://github.com/ariadne-cps/hybrid/actions/workflows/coverage.yml/badge.svg)](https://github.com/ariadne-cps/hybrid/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/ariadne-cps/hybrid/branch/main/graph/badge.svg)](https://codecov.io/gh/ariadne-cps/hybrid)

Ariadne Hybrid is the standalone C++20 hybrid-space representation layer used by Ariadne. It provides discrete locations, hybrid spaces, sets, constraints, grids, pavings, and graphical representations on top of Ariadne Labelled. Dynamical systems, hybrid automata, simulation, evolution, and reachability analysis belong to the separate System, Evolution, and Analysis layers.

## Layout

Public headers live under `include/hybrid`, implementations under `src`, and C++ tests under `tests`. The repository uses `ss` for its pinned Git submodules. Python bindings and smoke tests live under `python`. The C++ and Python tutorial programs live together in the top-level `tutorial/` directory.

The umbrella header is `include/ariadne-hybrid.hpp`.

## Dependencies

Hybrid depends directly on:

- [ariadne-cps/labelled](https://github.com/ariadne-cps/labelled), included as a Git submodule.
- [ariadne-cps/configuration](https://github.com/ariadne-cps/configuration), used for shared build configuration.
- [ariadne-cps/testing](https://github.com/ariadne-cps/testing), pinned at `ss/tst`, for C++ test fixtures and diagnostics only; not a production dependency.

Labelled supplies the lower Ariadne layers and their external dependencies. C++ unit tests use independent `Ariadne::Testing::ContextFixture` cases; the standalone graphics-output demonstration remains intentionally excluded from CTest.

## Build

```bash
git clone --recurse-submodules https://github.com/ariadne-cps/hybrid.git
cd hybrid
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target everything
ctest --test-dir build --output-on-failure
```

The C++ and Python examples are available in [the Hybrid tutorial](tutorial/README.md), which includes standalone build and run instructions.

## Python bindings

When a compatible Python development environment is available, the standalone build produces the combined `pyariadne` extension from the Labelled dependency and Hybrid bindings. The smoke test `python/tests/test_hybrid.py` checks discrete locations, hybrid spaces, grids, paving, and boxes.

## Contribution guidelines

If you would like to contribute to Ariadne Hybrid, please contact the developer:

- Luca Geretti <luca.geretti@univr.it>

## License

Ariadne Hybrid is released under the GNU General Public License v3.0.
