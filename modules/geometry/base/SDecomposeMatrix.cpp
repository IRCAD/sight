/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sight::module::geometry::base
{

// ----------------------------------------------------------------------------

SDecomposeMatrix::SDecomposeMatrix() noexcept =
    default;

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
    return {{s_SOURCE_INPUT, data::Object::s_MODIFIED_SIG, IService::slots::s_UPDATE}};
}

// ----------------------------------------------------------------------------

void SDecomposeMatrix::updating()
{
    auto matrix = m_source.lock();
    SIGHT_ASSERT("input matrix '" << s_SOURCE_INPUT << "' is not defined", matrix);

    glm::dmat4 glmMatrix = sight::geometry::data::getMatrixFromTF3D(*matrix);
    glm::dvec3 glmScale;
    glm::dquat orientation;
    glm::dvec3 glmTranslation;
    glm::dvec3 skew;
    glm::dvec4 perspective;

    /// Matrix decomposition
    glm::decompose(glmMatrix, glmScale, orientation, glmTranslation, skew, perspective);
    glm::dmat4 orientationMat = glm::toMat4(orientation);

    {
        auto rotation = m_rotation.lock();
        if(rotation)
        {
            sight::geometry::data::identity(*rotation);
            sight::geometry::data::setTF3DFromMatrix(*rotation, orientationMat);

            auto rotSig = rotation->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            rotSig->asyncEmit();
        }
    }
    {
        auto translation = m_translation.lock();
        if(translation)
        {
            sight::geometry::data::identity(*translation);
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                (*translation)(i, 3) = glmTranslation[int(i)];
            }

            auto transSig =
                translation->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            transSig->asyncEmit();
        }
    }

    auto scale = m_scale.lock();
    if(scale)
    {
        {
            sight::geometry::data::identity(*scale);
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                for(std::size_t j = 0 ; j < 3 ; j++)
                {
                    if(i == j)
                    {
                        (*scale)(i, j) = glmScale[int(i)];
                    }
                }
            }

            auto scaleSig = scale->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            scaleSig->asyncEmit();
        }
    }

    m_sigComputed->asyncEmit();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry::base
