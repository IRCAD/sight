/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "maths/SDecomposeMatrix.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace maths
{

const ::fwServices::IService::KeyType s_SOURCE_INPUT      = "source";
const ::fwServices::IService::KeyType s_TRANSLATION_INOUT = "translation";
const ::fwServices::IService::KeyType s_ROTATION_INOUT    = "rotation";
const ::fwServices::IService::KeyType s_SCALE_INOUT       = "scale";

// ----------------------------------------------------------------------------

SDecomposeMatrix::SDecomposeMatrix() noexcept
{
}

// ----------------------------------------------------------------------------

void SDecomposeMatrix::configuring()
{
}

// ----------------------------------------------------------------------------

void SDecomposeMatrix::starting()
{
}

// ----------------------------------------------------------------------------

void SDecomposeMatrix::stopping()
{
}

// ----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SDecomposeMatrix::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_SOURCE_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

void SDecomposeMatrix::updating()
{
    auto matrix = this->getInput< ::fwData::TransformationMatrix3D >(s_SOURCE_INPUT);
    SLM_ASSERT("input matrix '" + s_SOURCE_INPUT + "' is not defined", matrix);
    ::fwData::mt::ObjectReadLock srcLock(matrix);

    ::glm::dmat4 glmMatrix = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(matrix);
    ::glm::dvec3 scale;
    ::glm::dquat orientation;
    ::glm::dvec3 translation;
    ::glm::dvec3 skew;
    ::glm::dvec4 perspective;

    /// Matrix decomposition
    ::glm::decompose(glmMatrix, scale, orientation, translation, skew, perspective);
    ::glm::dmat4 orientationMat = ::glm::toMat4(orientation);

    auto rotation       = this->getInOut< ::fwData::TransformationMatrix3D >(s_ROTATION_INOUT);
    auto translationMat = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSLATION_INOUT);
    auto scaleMat       = this->getInOut< ::fwData::TransformationMatrix3D >(s_SCALE_INOUT);

    if( rotation)
    {
        ::fwData::mt::ObjectWriteLock rotLock(rotation);
        ::fwDataTools::TransformationMatrix3D::identity(rotation);
        ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(rotation, orientationMat);

        auto rotSig = rotation->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        rotSig->asyncEmit();
    }

    if( translationMat)
    {
        ::fwData::mt::ObjectWriteLock transLock(translationMat);
        ::fwDataTools::TransformationMatrix3D::identity(translationMat);
        for (size_t i = 0; i < 3; ++i)
        {
            translationMat->setCoefficient(i, 3, translation[i]);
        }
        auto transSig =
            translationMat->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        transSig->asyncEmit();
    }

    if( scaleMat)
    {
        ::fwData::mt::ObjectWriteLock scaleLock(scaleMat);
        ::fwDataTools::TransformationMatrix3D::identity(scaleMat);
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; j++)
            {
                if(i == j)
                {
                    scaleMat->setCoefficient(i, j, scale[i]);
                }
            }
        }
        auto scaleSig = scaleMat->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        scaleSig->asyncEmit();
    }

    m_sigComputed->asyncEmit();
}

// ----------------------------------------------------------------------------

}  // namespace maths
