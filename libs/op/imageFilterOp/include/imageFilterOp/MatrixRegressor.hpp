/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "imageFilterOp/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <glm/mat4x4.hpp>

namespace imageFilterOp
{

/**
 * @brief Computes an 'average' matrix from a matrix list.
 */
class IMAGEFILTEROP_CLASS_API MatrixRegressor
{
public:

    typedef ::glm::dvec4 PointType;

    /**
     * @brief Constructor
     * @pre The current implementation assumes that the matrices have a uniform scale.
     *
     * @param matrixList list of matrix to 'average'.
     * @param points list of points used for solving.
     */
    IMAGEFILTEROP_API MatrixRegressor(const ::fwData::Vector::csptr& matrixList, const std::vector<PointType>& points);

    /**
     * @brief computes the matrix that best fits our matrix list.
     *
     * @param initValue the initial value passed to the solver.
     * @param stepLength the initial step distance in a line direction.
     * @param stepTolerance terminates the optimization when the parameter is within this distance of the local minima.
     * @param valueTolerance terminates the optimization when the transformed distance between the matrices and our
     * current solution and the local extreme are likely to be within this tolerance.
     * @param maxIter the maximum number of iterations for the solver.
     * @return the 'average' matrix
     */
    IMAGEFILTEROP_API ::fwData::TransformationMatrix3D::sptr minimize(
        const ::fwData::TransformationMatrix3D::csptr& initValue,
        double stepLength     = 1.,
        double stepTolerance  = 1e-2,
        double valueTolerance = 1e-2,
        unsigned int maxIter = 10000);

private:

    std::vector< ::glm::dmat4 > m_matList;

    std::vector< PointType > m_pointList;

};

} // namespace imageFilterOp.
