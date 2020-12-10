/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "imageFilterOp/MatrixRegressor.hpp"

#include <imageFilterOp/PowellOptimizer.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>

namespace imageFilterOp
{

//-----------------------------------------------------------------------------

MatrixRegressor::MatrixRegressor(const ::fwData::Vector::csptr& matrixList, const std::vector<PointType>& points) :
    m_pointList(points)
{
    for(const auto& elt : *matrixList)
    {
        ::fwData::TransformationMatrix3D::sptr mat = std::dynamic_pointer_cast< ::fwData::TransformationMatrix3D >(elt);

        m_matList.push_back(::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(mat));
    }
}

//-----------------------------------------------------------------------------

fwData::TransformationMatrix3D::sptr MatrixRegressor::minimize(const ::fwData::TransformationMatrix3D::csptr& initValue,
                                                               double stepLength, double stepTolerance,
                                                               double valueTolerance, unsigned int maxIter)
{
    ::glm::dmat4 initMat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(initValue);
    double scale = std::pow(::glm::determinant(initMat), 1./3.);

    ::glm::dvec3 angles = ::glm::eulerAngles(::glm::toQuat(initMat / scale));

    ::glm::dvec3 translation = ::glm::dvec3(::glm::column(initMat, 3));

    PowellOptimizer::FunctionParametersType initParams(7);
    initParams.put(0, translation[0]);
    initParams.put(1, translation[1]);
    initParams.put(2, translation[2]);
    initParams.put(3, angles[0]);
    initParams.put(4, angles[1]);
    initParams.put(5, angles[2]);
    initParams.put(6, scale);

    PowellOptimizer::OptimizedFunctionType distanceSum =
        [this](const PowellOptimizer::FunctionParametersType& parameters)
        {
            ::glm::dmat4 M = ::glm::eulerAngleYXZ(parameters[4], parameters[3], parameters[5]);
            M              = ::glm::translate(M, ::glm::dvec3(parameters[0], parameters[1], parameters[2]));
            M              = ::glm::scale(M, ::glm::dvec3(parameters[6], parameters[6], parameters[6]));

            double distance = 0;

            for(const ::glm::dmat4& m : this->m_matList)
            {
                for(const MatrixRegressor::PointType& p : this->m_pointList)
                {
                    distance += ::glm::distance2(m * p, M * p);
                }
            }

            return distance;
        };

    PowellOptimizer optimizer(distanceSum, stepTolerance, valueTolerance, stepLength, maxIter);

    PowellOptimizer::FunctionParametersType finalPosition = optimizer.optimize(initParams);

    ::glm::dmat4 result = ::glm::eulerAngleYXZ(finalPosition[4], finalPosition[3], finalPosition[5]);
    result              = ::glm::translate(result, ::glm::dvec3(finalPosition[0], finalPosition[1], finalPosition[2]));
    result              = ::glm::scale(result, ::glm::dvec3(finalPosition[6], finalPosition[6], finalPosition[6]));

    ::fwData::TransformationMatrix3D::sptr resMat = ::fwData::TransformationMatrix3D::New();
    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(resMat, result);

    return resMat;
}

//-----------------------------------------------------------------------------

} // namespace imageFilterOp.
