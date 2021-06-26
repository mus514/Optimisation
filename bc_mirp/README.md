# Multi-vehicle Inventory-routing problem (MIRP) Branch-and-cut algorithm using CVRPSEP package

A C++ implementation of the MIRP [[1](#references)] Branch-and-cut using Gurobi's API and CVRPSEP package [[2](#references)].

## Prerequisites

* CMake.

* C++17 compiler or an early version.

* GUROBI solver (9 or an early version). Academics can obtain it via this [link](https://www.gurobi.com/downloads/gurobi-optimizer-eula/#Reg "Gurobi's register page").

## Compile and run instructions

Go to the source code folder and to compile type:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

for the debug version or simply

```sh
cmake -H. -Bbuild
cmake --build build
```

for the release version.

To run with a configuration file:

```sh
$ ./build/bc_mirp -f [configuration file path]
```

See the "example.cfg" file at the "input" folder for an example of the input configuration file.

## References

**[\[1\] L. C. Coelho, J.-F. Cordeau and G. Laporte. Consistency in multi-vehicle inventory-routing. Transportation Research Part C, 24(1), 2012, pp. 270–87.](https://www.sciencedirect.com/science/article/abs/pii/S0968090X12000502)**

**[\[2\] J. Lysgaard, A.N. Letchford and R.W. Eglese. A New Branch-and-Cut Algorithm for the Capacitated Vehicle Routing Problem. Mathematical Programming, vol. 100 (2), pp. 423-445](https://pubsonline.informs.org/doi/10.1287/trsc.1060.0188)**
