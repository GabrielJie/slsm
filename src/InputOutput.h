/*
  Copyright (c) 2015-2017 Lester Hedges <lester.hedges+slsm@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INPUTOUTPUT_H
#define _INPUTOUTPUT_H

#include <sstream>
#include <string>

/*! \file InputOutput.h
    \brief A class for reading and writing data.
 */

namespace slsm
{
    // FORWARD DECLARATIONS

    class Boundary;
    class LevelSet;
    class Mesh;

    //! A class for reading and writing data.
    class InputOutput
    {
    public:
        //! Constructor.
        InputOutput();

        //! Save the level set function as a ParaView VTK file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param levelSet
                A reference to the level set object.

            \param isVelocity
                Whether to write velocity information to file (optional).

            \param isGradient
                Whether to write gradient information to file (optional).

            \param outputDirectory
                The output directory path (optional).
         */
        void saveLevelSetVTK(const unsigned int&, const LevelSet&, bool isVelocity = false,
            bool isGradient = false, const std::string& outputDirectory = "") const;

        //! Save the level set function as a ParaView VTK file.
        /*! \param fileName
                The name of the data file.

            \param levelSet
                A reference to the level set object.

            \param isVelocity
                Whether to write velocity information to file (optional).

            \param isGradient
                Whether to write gradient information to file (optional).
         */
        void saveLevelSetVTK(const std::string&, const LevelSet&,
            bool isVelocity = false, bool isGradient = false) const;

        //! Save the level set function as a plain text file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param levelSet
                A reference to the level set object.

            \param outputDirectory
                The output directory path (optional).

            \param isXY
                Whether to also output the nodal x/y coordinates (optional).
         */
        void saveLevelSetTXT(const unsigned int&, const LevelSet&,
            const std::string& outputDirectory = "", bool isXY = false) const;

        //! Save the level-set signed distance function as a binary file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param levelSet
                A reference to the level set object.

            \param outputDirectory
                The output directory path (optional).
         */
        void saveLevelSetBIN(const unsigned int&, const LevelSet&,
            const std::string& outputDirectory = "") const;

        //! Save the level set function as a plain text file.
        /*! \param fileName
                The name of the data file.

            \param levelSet
                A reference to the level set object.

            \param isXY
                Whether to also output the nodal x/y coordinates (optional).
         */
        void saveLevelSetTXT(const std::string&, const LevelSet&, bool isXY = false) const;

        //! Save the level-set signed distance function as a binary file.
        /*! \param fileName
                The name of the data file.

            \param levelSet
                A reference to the level set object.
         */
        void saveLevelSetBIN(const std::string&, const LevelSet&) const;

        //! Load the level-set signed distance function from a plain text file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param levelSet
                A reference to the level set object.

            \param inputDirectory
                The input directory path (optional).

            \param isXY
                Whether the data file also contains nodal x/y coordinates (optional).
         */
        void loadLevelSetTXT(const unsigned int&, LevelSet&,
            const std::string& inputDirectory = "", bool isXY = false) const;

        //! Load the level-set signed distance function from a binary file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param levelSet
                A reference to the level set object.

            \param inputDirectory
                The input directory path (optional).
         */
        void loadLevelSetBIN(const unsigned int&, LevelSet&,
            const std::string& inputDirectory = "") const;

        //! Load the level set function from a plain text file.
        /*! \param fileName
                The name of the data file.

            \param levelSet
                A reference to the level set object.

            \param isXY
                Whether the data file also contains nodal x/y coordinates (optional).
         */
        void loadLevelSetTXT(const std::string&, LevelSet&, bool isXY = false) const;

        //! Load the level-set signed distance function from a binary file.
        /*! \param fileName
                The name of the data file.

            \param levelSet
                A reference to the level set object.
         */
        void loadLevelSetBIN(const std::string&, LevelSet&) const;

        //! Save boundary points as a plain text file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param boundary
                A reference to the boundary object.

            \param outputDirectory
                The output directory path (optional).
         */
        void saveBoundaryPointsTXT(const unsigned int&, const Boundary&,
            const std::string& outputDirectory = "") const;

        //! Save boundary points as a plain text file.
        /*! \param fileName
                The name of the data file.

            \param boundary
                A reference to the boundary object.
         */
        void saveBoundaryPointsTXT(const std::string&, const Boundary&) const;

        //! Save boundary segments as a plain text file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param boundary
                A reference to the boundary object.

            \param outputDirectory
                The output directory path (optional).
         */
        void saveBoundarySegmentsTXT(const unsigned int&,
            const Boundary&, const std::string& outputDirectory = "") const;

        //! Save boundary segments as a plain text file.
        /*! \param fileName
                The name of the data file.

            \param boundary
                A reference to the boundary object.
         */
        void saveBoundarySegmentsTXT(const std::string&, const Boundary&) const;

        //! Save the element area fractions as a ParaView VTK file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param mesh
                A reference to the level set mesh.

            \param outputDirectory
                The output directory path (optional).
         */
        void saveAreaFractionsVTK(const unsigned int&, const Mesh&,
            const std::string& outputDirectory = "") const;

        //! Save the element area fractions as a ParaView VTK file.
        /*! \param fileName
                The name of the data file.

            \param mesh
                A reference to the level set mesh.
         */
        void saveAreaFractionsVTK(const std::string&, const Mesh&) const;

        //! Save element area fractions as a plain text file.
        /*! \param datapoint
                The datapoint of the current optimisation trajectory.

            \param mesh
                A reference to the level set mesh.

            \param outputDirectory
                The output directory path (optional).

            \param isXY
                Whether to also output the element x/y coordinates (optional).
         */
        void saveAreaFractionsTXT(const unsigned int&, const Mesh&,
            const std::string& outputDirectory = "", bool isXY = false) const;

        //! Save element area fractions as a plain text file.
        /*! \param fileName
                The name of the data file.

            \param mesh
                A reference to the level set mesh.

            \param isXY
                Whether to also output the element x/y coordinates (optional).
         */
        void saveAreaFractionsTXT(const std::string&, const Mesh&, bool isXY = false) const;
    };
}

#endif  /* _INPUTOUTPUT_H */
