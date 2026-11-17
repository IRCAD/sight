/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
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

#include "module/geometry/__/decompose_matrix.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sight::module::geometry
{

// ----------------------------------------------------------------------------

decompose_matrix::decompose_matrix() noexcept :
    filter(has_signals::signals())
{
}

// ----------------------------------------------------------------------------

void decompose_matrix::configuring()
{
}

// ----------------------------------------------------------------------------

void decompose_matrix::starting()
{
}

// ----------------------------------------------------------------------------

void decompose_matrix::stopping()
{
}

// ----------------------------------------------------------------------------

service::connections_t decompose_matrix::auto_connections() const
{
    return {{SOURCE_INPUT, data::signals::MODIFIED, service::slots::UPDATE}};
}

// ----------------------------------------------------------------------------

void decompose_matrix::updating()
{
    auto matrix = m_source.lock();
    SIGHT_ASSERT("input matrix '" << SOURCE_INPUT << "' is not defined", matrix);

    glm::dmat4 glm_matrix = sight::geometry::data::to_glm_mat(*matrix);
    glm::dvec3 glm_scale {};
    glm::dquat orientation {};
    glm::dvec3 glm_translation {};
    glm::dvec3 skew;
    glm::dvec4 perspective;

    /// Matrix decomposition
    glm::decompose(glm_matrix, glm_scale, orientation, glm_translation, skew, perspective);
    glm::dmat4 orientation_mat = glm::toMat4(orientation);

    {
        auto rotation = m_rotation.lock();
        if(rotation)
        {
            sight::geometry::data::identity(*rotation);
            sight::geometry::data::from_glm_mat(*rotation, orientation_mat);

            rotation->async_emit(data::signals::MODIFIED);
        }
    }
    {
        auto translation = m_translation.lock();
        if(translation)
        {
            sight::geometry::data::identity(*translation);
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                (*translation)(i, 3) = glm_translation[static_cast<int>(i)];
            }

            translation->async_emit(data::signals::MODIFIED);
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
                        (*scale)(i, j) = glm_scale[static_cast<int>(i)];
                    }
                }
            }

            scale->async_emit(data::signals::MODIFIED);
        }
    }

    this->async_emit(signals::SUCCEEDED);
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry
