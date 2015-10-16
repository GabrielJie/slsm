/*
 * File:	FastMarchingMethod.h
 * Author:	lester
 */

// Adapted from Scikit-FMM: https://github.com/scikit-fmm/scikit-fmm

#ifndef _FASTMARCHINGMETHOD_H
#define _FASTMARCHINGMETHOD_H

#include <limits>

#include "Debug.h"
#include "Heap.h"
#include "LevelSet.h"

/*! \file FastMarchingMethod.h
    \brief A implementation of a Fast Marching Method for finding
    approximate solutions to boundary value problems of the Eikonal
    equation:

        F(x) | grad T(x) | = 1

    This object can be used to reinitialise the level set to a signed
    distance function, or to compute extension velocities using know
    values at the boundary.
 */

//! Whether a node lies inside, outside, or on the boundary.
namespace FMM_NodeStatus
{
    // Left bit shift enumerated types to allow the creation
    // of sets and simple bit masking operations.
    enum FMM_NodeStatus
    {
        NONE            = 0,                //!< No status.
        FROZEN          = (1 << 0),         //!< Node lies on boundary or has been frozen.
        TRIAL           = (1 << 1),         //!< Node is adjacent to a frozen node.
        MASKED          = (1 << 2),         //!< Node has been masked.
    };
}

class FastMarchingMethod
{
public:
    //! Constructor.
    /*! \param mesh_
            A reference to the finite element mesh.

        \param isTest_
            Whether to test the heap following each update.
     */
    FastMarchingMethod(const Mesh&, bool isTest_=false);

    //! Destructor.
    ~FastMarchingMethod();

    void march(std::vector<double>&);
	void march(std::vector<double>&, std::vector<double>&);

private:
    /// A const reference to the finite element mesh.
    const Mesh& mesh;

    /// The ascending unsigned distance priority queue.
    Heap *heap;

    /// Back pointers to the heap.
    std::vector<unsigned int> heapPtr;

    /// Whether to test the heap after each update.
    bool isTest;

    /// Whether velocity extension is active (distance extension if not).
    bool isVelocity;

    /// Out of bounds neighbour flag.
    unsigned int outOfBounds;

    /// The status of each node.
    std::vector<FMM_NodeStatus::FMM_NodeStatus> nodeStatus;

    /// A copy of the initial signed distance function.
    std::vector<double> signedDistanceCopy;

    /// A copy of the initial velocity field.
    std::vector<double> velocityCopy;

    /// A pointer to the signed distance vector.
    std::vector<double>* signedDistance;

    /// A pointer to the velocity vector.
    std::vector<double>* velocity;

    //! Find boundary nodes and flag them as frozen.
    void initialiseFrozen();

    //! Initialise the heap data structure.
    void initialiseHeap();

    //! Initialise the trial set. Find nodes adjacent to the boundary
    // ! and mark them as trial nodes.
    void initialiseTrial();

    //! Obtain the fast marching solution.
    void solve();

    //! Update the value at a node.
    /*! \param node
            The index of the node.

        \return
            The new value (distance or velocity) at the node.
     */
    double updateNode(unsigned int);

    //! Finalise the velocity at a node.
    /*! \param node
            The index of the node.
     */
    void finaliseVelocity(unsigned int node);

    //! Solve the quadratic equation.
    /*! \param node
            The index of the node.

        \param a
            The first quadratic coefficient.

        \param b
            The second quadratic coefficient.

        \param c
            The third quadratic coefficient.

        \return
            The correct root of the equation.
     */
    double solveQuadratic(unsigned int, const double&, const double&, const double&) const;

    const double doubleEpsilon = std::numeric_limits<double>::epsilon();
    const double maxDouble = std::numeric_limits<double>::max();
};

#endif	/* _FASTMARCHINGMETHOD_H */
