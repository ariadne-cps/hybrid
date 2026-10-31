# Ariadne Hybrid tutorial

The C++ program in `tutorial/` and Python program in `python/tutorial/`
show what the **Hybrid** representation layer provides, without constructing a dynamical system or running evolution
or reachability algorithms.

The two programs cover the same basic ideas: discrete locations, continuous
state variables that differ by location, hybrid spaces, boxes, grids and
pavings. The C++ example also builds an outer approximation of a bounded
hybrid constraint set. The Python example uses the subset of this interface
currently exposed by the standalone `pyariadne` bindings.

## Build the library

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel --target everything
cmake --install build --prefix install
```

Python bindings require a compatible Python development environment.

## C++ tutorial

Build the tutorial **separately**, against the installed library:

```bash
cmake -S tutorial -B tutorial/build -DCMAKE_PREFIX_PATH="$PWD/install"
cmake --build tutorial/build --parallel
./tutorial/build/tutorial_hybrid
```

On Windows, run `tutorial/build/tutorial_hybrid.exe`. A shared-library
installation may need its `lib` directory on the runtime library search
path (or `bin` on Windows), as with the Symbolic tutorial.

## Python tutorial

Run the installed `pyariadne` extension. For a non-system install, add
`install/lib/pythonX.Y/site-packages` (or
`install/Lib/site-packages` on Windows) to `PYTHONPATH` and ensure the
library loader can locate the installed shared libraries, then run:

```bash
python python/tutorial/tutorial_hybrid.py
```

This program is executable as an ordinary script; it doesn't rely on pytest.
