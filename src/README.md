# Class Overview

LibSLSM is built around five main classes:

- [Boundary](#boundary)
- [LevelSet](#levelset)
- [Mesh](#mesh)
- [Optimise](#optimise)
- [Sensitivity](#sensitivity)

Note that objects of other class types are referred to in the above sections
in camelCase, e.g. a `LevelSet` object will be referred to in the
documentation for other classes as `levelSet`.

Several support classes provide additional functionality:

- [Hole](#hole)
- [InputOutput](#inputoutput)
- [MersenneTwister](#mersennetwister)

## Boundary

This class provides functionality for the piece-wise linear discretisation
of the zero contour of the level set. To initialise an object and compute the
discretised boundary:

```cpp
// Initialise the boundary object.
slsm::Boundary boundary;

// Generate the discretisation of the zero contour.
boundary.discretise(levelSet);
```

When performing shape matching simulations the target shape may be discretised
as follows:

```cpp
boundary.discretise(levelSet, true);
```

Following discretisation, the total length of the boundary may be accessed
using the `length` member variable, e.g.

```cpp
// Print the boundary length.
std::cout << boundary.length << '\n';
```

Individual boundary points are contained in the `points` member vector. To
print the length associated with each point:

```cpp
// Print the boundary point lengths.
for (unsigned int i=0;i<boundary.points.size();i++)
  std::cout << boundary.points[i].length << '\n';
```

To compute inward pointing normal vectors for the set of the boundary points:

```cpp
boundary.computeNormalVectors(levelSet);
```

The x and y components of the normal vectors can be accessed as follows:

```cpp
// Print the boundary point normal vectors.
for (unsigned int i=0;i<boundary.points.size();i++)
{
  std::cout << boundary.points[i].normal.x << ' '
            << boundary.points[i].normal.y << '\n';
}
```

See [Boundary.h](Boundary.h) and [Boundary.cpp](Boundary.cpp) for further
implementation details.

## LevelSet

This class provides functionality for the level set domain. We use a fixed-grid
[Mesh](#mesh) to represent a non-periodic, rectangular design domain. The mesh
represents a discretisation of the level set, or implicit function. At each
node of the mesh we store the signed distance from the nearest interface, i.e.
the closest point on the zero contour of the level set. A set of methods are
provided for initialising, maintaining, and updating the level set.

### Initialisation
There are many ways to initialise the level set domain, providing flexibility
for a range of different use cases.

#### 1) Default

To initialise a 200 by 200 level-set domain:

```cpp
slsm::LevelSet levelSet(200, 200);
```

Here the signed distance function will be initialised using a tiled array
of holes, as is commonly used in may compliance minimisation simulations.
The are several optional parameters that may be passed to the constructor,
allowing the user to specify the
[Courant-Friedrichs-Lewy](https://en.wikipedia.org/wiki/Courant–Friedrichs–Lewy_condition)
(CFL) condition, to adjust the width of the narrow band region, or to fix
the boundary of the domain. Default parameters are a CFL limit of 0.5 grid
units, a narrow band of 6 elements, and a free domain boundary.

To use a CFL limit of 0.1 grid units, a bandwidth of 10 elements, and to
pin boundary points on the domain boundary:

```cpp
slsm::LevelSet levelSet(200, 200, 0.1, 10, true);
```

#### 2) Holes

It is also possible to initialise the level set using a custom arrangement
of [Hole](#hole) objects:

```cpp
// Initialise a vector of holes.
std::vector<slsm::Hole> holes;

// Push two holes into the vector.
holes.push_back(slsm::Hole(50,  50, 20);
holes.push_back(slsm::Hole(150, 50, 20);

// Initalise the level set.
slsm::LevelSet levelSet(200, 200, holes);
```

#### 3) Points

Alternatively, the level set can be initialised using a vector of points that
define a single piece-wise linear shape. The points must be clockwise-ordered
and closed. For example, to initialise the level set using a square:

```cpp
// Initialise a vector of points.
std::vector<slsm::Coord> points;

// Create the square (clockwise-ordered and closed).
points.push_back(slsm::Coord(50,  50);
points.push_back(slsm::Coord(50,  150);
points.push_back(slsm::Coord(150, 150);
points.push_back(slsm::Coord(150, 50);
points.push_back(slsm::Coord(50,  50);

// Initalise the level set.
slsm::LevelSet levelSet(200, 200, points);
```

#### 4) Shape Matching

Support is provided for shape matching simulations where the level set converges
towards a predefined target shape. The following example shows how to
initialise the level set using a hole and match against a hexagon.

```cpp
// Initialise a vector of holes.
std::vector<slsm::Hole> holes;

// Add a single hole with a radius of 60 grid units.
hole.push_back(slsm::Hole(100, 100, 60);

// Initialise a vector of points.
std::vector<slsm::Coord> points;

// Create a hexagon with the same radius.
points.push_back(lsm::Coord(70,  48));
points.push_back(lsm::Coord(40,  100));
points.push_back(lsm::Coord(70,  152));
points.push_back(lsm::Coord(130, 152));
points.push_back(lsm::Coord(160, 100));
points.push_back(lsm::Coord(130, 48));
points.push_back(lsm::Coord(70,  48));

// Initalise the level set.
// (Fix the domain boundary so that it's not considered as part of the interface.)
slsm::LevelSet levelSet(200, 200, holes, points, 0.5, 6, true);
```

It's possible to use any combination of holes and points to initialise the
signed-distance functions of the level-set and target.

#### 5) Manual

It is also possible to manually initialise the signed distance function, e.g.

```cpp
// Initialise a 200x200 level set domain.
slsm::LevelSet levelSet(200, 200, 0.5, 6, true);

// Create a solid slab of material with a small square in the middle.
for (unsigned int i=0;i<levelSet.mesh.nNodes;i++)
{
  unsigned int x = int(levelSet.mesh.nodes[i].coord.x);
  unsigned int y = int(levelSet.mesh.nodes[i].coord.y);

  // Cut out the square hole.
  if (x >= 80 && x <= 120 && y >= 80 && y <= 120)
	levelSet.signedDistance[i] = -1;

  // Fill everywhere else with material.
  else levelSet.signedDistance[i] = 1;
}

// Reinitialise the level set to a signed distance function.
levelSet.reinitialise();
```

### Updating

There are several steps that go into updating the level set:

#### 1) Velocities

For a traditional, zero temperature level-set simulation, boundary point
velocities can be extended to the nodes of the level set as follows:

```cpp
levelSet.computeVelocities(boundary.points);
```

For a stochastic level-set simulation, the same update would be achieved by:

```cpp
// Initialise the random number generator.
slsm::MersenneTwister rng;

// Set the temperature.
double temperature = 0.1;

// Extend the boundary point velocities.
levelSet.computeVelocities(boundary.points, timeStep, temperature, rng);
```

Here the `timeStep` parameter is an output from the optimiser. See the
[Optimise](#optimise) class documentation for details.

#### 2) Gradients

Absolute gradients of the signed distance function can be computed as follows:

```cpp
levelSet.computeGradients();
```

The gradient is calculated using the 5th order Hamilton-Jacobi Weighted
Essentially Non-Oscillatory (HJ-WENO) approximation taken from the book
[Level Set Methods and Dynamic Implicit Surfaces](http://www.springer.com/gp/book/9780387954820).

#### 3) Signed Distance

Once velocities and gradients have been calculated then the signed distance
function can be updated:

```cpp
bool isReinitialise = levelSet.update(timeStep);
```

Once again, the `timeStep` parameter is an output from the [optimiser](#optimise).

The return value of the update method indicates whether the zero contour of
the level set has reached the edge of the current narrow band region,
triggering a reinitialisation. Manual reinitialisation of the signed distance
function and narrow band can be done as follows:

```cpp
levelSet.reinitialise();
```

### Area Fractions

For many problems one needs to know the area of the level-set domain that
is enclosed by the discretised zero contour. Having generated a discretisation
of the zero contour using a [Boundary](#boundary) object, this can be done by:

```cpp
double area = levelSet.computeAreaFractions(boundary);
```

The `computeAreaFractions` method returns the total area fraction. Individual
mesh element area fractions can be accessed using:

```cpp
// Store the total area fration.
double area = levelSet.computeAreaFractions(boundary);

// Print all of the individual element area fractions.
for (unsigned int i=0;i<levelSet.mesh.nElements;i++)
  std::cout << levelSet.mesh.elements[i].area << '\n';
```

See [LevelSet.h](LevelSet.h) and [LevelSet.cpp](LevelSet.cpp) for further
implementation details.

## Mesh

The mesh represents a two-dimensional rectangular design domain comprised of
fixed, unit square elements. The mesh is non-periodic and dimensionless.
If you require units in your application, simply assign a physical dimension
to the element edge length in your calculations.

The Mesh class provides useful bookkeeping functionality for the mapping
between elements and nodes of the level set domain. Each [LevelSet](#levelset)
object stores its mesh as a member and most Mesh operations should be hidden
from the user.

To instantiate a 200 by 200 Mesh object:

```cpp
slsm::Mesh mesh(200, 200);
```

To find the node closest to a specific (x, y) coordinate:

```cpp
double x = 10.2;
double y = 22.6;
unsigned int node = mesh.getClosestNode(x, y);
```

Alternatively, using the slsm::Coord data structure:

```cpp
slsm::Coord coord(10.2, 22.6);
unsigned int node = mesh.getClosestNode(coord);
```

To find the element that contains a point:

```cpp
double x = 68.7;
double y = 13.1;
unsigned int element = mesh.getElement(x, y);
```

Similarly:

```cpp
slsm::Coord coord(68.7, 13.1);
unsigned int element = mesh.getElement(coord);
```

See [Mesh.h](Mesh.h) and [Mesh.cpp](Mesh.cpp) for further
implementation details.

## Optimise

The Optimise class provides a modified implementation of the
[Sequential Linear Programming](http://link.springer.com/article/10.1007/s00158-014-1174-z)
optimisation method developed by Dunning and Kim. The optimiser finds the set
of boundary point velocities that produce the largest change in the objective
function, subject to satisfying any constraints. Support is provided for
minimising or maximising the objective function, and for an arbitrary number
of equality and inequality constraints. The class makes use of the
[NLopt](http://ab-initio.mit.edu/wiki/index.php/NLopt) package.
See the [Dependencies](../README.md) section for details. The optimiser assumes
that boundary point sensitivities have been calculated for all of the objective
and constraint functions. See the [Sensitivity](#sensitivity) documentation for
details on computing sensitivities.

As an example, let's consider the minimisation of an arbitrary objective
function subject to a single inequality constraint: a lower bound on the
area fraction. The optimisation problem can be set up and solved as follows:

```cpp
// Minimum area fraction.
double minArea = 0.6;

// Time step associated with the iteration.
double timeStep;

// Lambda vector.
std::vector<double> lambdas(2);

// Constraint distance vector.
std::vector<double> constraintDistances;

// Push current distance from constraint into vector.
constraintDistances.push_back(meshArea*minArea - levelSet.area);

// Initialise the optimisation object.
slsm::Optimise optimise(boundary.points, constraintDistances,
  lambdas, timeStep, levelSet.moveLimit);

// Perform the optimisation.
optimise.solve()
```

If we instead wish to maximise the objective function subject to an equality
constraint:

```cpp
// Minimum area fraction.
double minArea = 0.6;

// Time step associated with the iteration.
double timeStep;

// Lambda vector.
std::vector<double> lambdas(2);

// Constraint distance vector.
std::vector<double> constraintDistances;

// Push current distance from constraint into vector.
constraintDistances.push_back(meshArea*minArea - levelSet.area);

// Whether each constraint is an equality.
std::vector<bool> isEquality;

// Add equality constraint.
isEquality.push_back(true);

// Initialise the optimisation object.
slsm::Optimise optimise(boundary.points, constraintDistances,
  lambdas, timeStep, levelSet.moveLimit, true, isEquality);

// Perform the optimisation.
optimise.solve()
```

Once a solution has been found the optimum boundary point velocities can be
accessed as follows:

```cpp
// Print the boundary point velocities.
for (unsigned int i=0;i<boundary.points.size();i++)
  std::cout << boundary.points[i].velocity << '\n';
```

See [Optimise.h](Optimise.h) and [Optimise.cpp](Optimise.cpp) for further
implementation details.

## Sensitivity

This class provides functionality for generic finite-difference boundary
point sensitivity calculations. Informally, the boundary point sensitivity
describes the rate of change of the objective function associated with moving
the boundary point in the inward-pointing normal direction.

The Sensitivity class provides a callback interface to allow the calculation
of sensitivities for an arbitrary function. The `SensitivityCallback` must take
a `BoundaryPoint` as an input argument and return the value of the function
associated with the point at its current position. As an example, let's consider
minimising the perimeter of a shape. Here the callback function measures the
length of the interface around the boundary point. The resulting sensitivity,
the rate of change of the perimeter per unit length of the boundary, is the
local curvature of the interface. The following code shows how to set up and
compute the required sensitivities. Here we assume that the sensitivities
correspond to the objective function, i.e. the zeroth sensitivity for each
boundary point.

```cpp
// Compute the boundary point normal vectors.
boundary.computeNormalVectors(levelSet);

// Initialise the sensitivity object.
slsm::Sensitivity sensitivity;

// Initialise the sensitivity callback.
using namespace std::placeholders;
slsm::SensitivityCallback callback = std::bind(&slsm::Boundary::computePerimeter, boundary, _1);

// Assign boundary point sensitivities.
for (unsigned int i=0;i<boundary.points.size();i++)
{
  boundary.points[i].sensitivities[0] =
    sensitivity.computeSensitivity(boundary.points[i], callback);
}
```

For some functions it is possible to analytically calculate boundary point
sensitivities. For example, the sensitivity associated with minimising or
maximising the area of a shape is simply plus or minus one, i.e. the change
in area when moving a section of the interface in the normal direction is
simply the length of that section, so the change per unit length is one.

```cpp
// Assign area constraint sensitivities.
for (unsigned int i=0;i<boundary.points.size();i++)
    boundary.points[i].sensitivities[1] = -1;
```

For stochastic simulations it is necessary to apply a correction to the
objective sensitivities. This is the deterministic part of the Ito
correction. If boundary point normal vectors have already been computed,
then this can be achieved as follows:

```cpp
sensitivity.itoCorrection(boundary, temperature);
```

Otherwise, the following should be used:

```cpp
sensitivity.itoCorrection(boundary, levelSet, temperature);
```

See [Sensitivity.h](Sensitivity.h) and [Sensitivity.cpp](Sensitivity.cpp) for
further implementation details.

## Hole

The Hole class provides a simple data type for circular holes. These can be
used when initialising the signed distance function of the [LevelSet](#levelset).

To create a hole:

```cpp
// Assign the coordinates.
double x = 10;
double y = 20;

// Assign the radius.
double r = 20;

// Initialise the hole.
slsm::Hole hole(x, y, r);
```

Alternatively, using the `Coord` data structure:

```cpp
// Initialise the coordinate container.
slsm::Coord coord(10, 20);

// Assign the radius.
double r = 20;

// Initialise the hole.
slsm::Hole hole(coord, r);
```

## InputOutput

The InputOutput class provides functionality for reading and writing data.
Data can be writting in plain text, `*.txt`, or in a
[ParaView](http://www.paraview.org) compatible `VTK` format.

To instantiate an object and write level set, boundary segment, and area
fraction information to file:

```cpp
// Instantiate InputOutput object.
slsm::InputOutput io;

// Save current signed distance information in text and VTK format.
io.saveLevelSetTXT(1, levelSet);
io.saveLevelSetVTK(1, levelSet);

// Save boundary segment data to a text file.
io.saveBoundarySegmentsTXT(1, boundary);

// Save area fraction information in VTK format.
io.saveAreaFractionsVTK(1, levelSet.mesh);
```

Here the first argument specifies the index of the data file, which is used to
generate a file prefix. This is useful when saving multiple data sets in a time
series.

See [InputOutput.h](InputOutput.h) and [InputOutput.cpp](InputOutput.cpp) for
further implementation details.

## MersenneTwister

This class provides a C++11 implementation of the
[Mersenne Twister](http://en.wikipedia.org/wiki/Mersenne_Twister)
psuedorandom number generator.

Here are some examples of how it may be used:

```cpp
// Initialise a random number generator.
slsm::MersenneTwister rng;

// Generate a uniform random double between 0 and 1.
double r1 = rng();

// Generate a uniform random integer between 7 and 36.
double r2 = rng.integer(7, 36);

// Draw a random double from a normal distrubution
// with zero mean and unit variance.
double r3 = rng.normal();

// Draw a random double from a normal distrubution
// with a mean of 10 and variance of 3.
double r4 = rng.normal(10, 3);
```

See [MersenneTwister.h](MersenneTwister.h) for further implementation details.
