/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "modules/geometry/base/SDecomposeMatrix.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <geometry/data/Matrix4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sight::module::geometry::base
{

const service::IService::KeyType s_SOURCE_INPUT      = "source";
const service::IService::KeyType s_TRANSLATION_INOUT = "translation";
const service::IService::KeyType s_ROTATION_INOUT    = "rotation";
const service::IService::KeyType s_SCALE_INOUT       = "scale";

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

service::IService::KeyConnectionsMap SDecomposeMatrix::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_SOURCE_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

void SDecomposeMatrix::updating()
{
    auto matrix = this->getInput<data::Matrix4>(s_SOURCE_INPUT);
    SIGHT_ASSERT("input matrix '" + s_SOURCE_INPUT + "' is not defined", matrix);
    data::mt::ObjectReadLock srcLock(matrix);

    ::glm::dmat4 glmMatrix = sight::geometry::data::getMatrixFromTF3D(matrix);
    ::glm::dvec3 scale;
    ::glm::dquat orientation;
    ::glm::dvec3 translation;
    ::glm::dvec3 skew;
    ::glm::dvec4 perspective;

    /// Matrix decomposition
    ::glm::decompose(glmMatrix, scale, orientation, translation, skew, perspective);
    ::glm::dmat4 orientationMat = ::glm::toMat4(orientation);

    auto rotation       = this->getInOut<data::Matrix4>(s_ROTATION_INOUT);
    auto translationMat = this->getInOut<data::Matrix4>(s_TRANSLATION_INOUT);
    auto scaleMat       = this->getInOut<data::Matrix4>(s_SCALE_INOUT);

    if(rotation)
    {
        data::mt::ObjectWriteLock rotLock(rotation);
        sight::geometry::data::identity(*rotation);
        sight::geometry::data::setTF3DFromMatrix(rotation, orientationMat);

        auto rotSig = rotation->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        rotSig->asyncEmit();
    }

    if(translationMat)
    {
        data::mt::ObjectWriteLock transLock(translationMat);
        sight::geometry::data::identity(*translationMat);
        for(size_t i = 0 ; i < 3 ; ++i)
        {
            translationMat->setCoefficient(i, 3, translation[i]);
        }

        auto transSig =
            translationMat->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        transSig->asyncEmit();
    }

    if(scaleMat)
    {
        data::mt::ObjectWriteLock scaleLock(scaleMat);
        sight::geometry::data::identity(*scaleMat);
        for(size_t i = 0 ; i < 3 ; ++i)
        {
            for(size_t j = 0 ; j < 3 ; j++)
            {
                if(i == j)
                {
                    scaleMat->setCoefficient(i, j, scale[i]);
                }
            }
        }

        auto scaleSig = scaleMat->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        scaleSig->asyncEmit();
    }

    m_sigComputed->asyncEmit();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry::base
