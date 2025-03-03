/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

// cspell:ignore slerp lerp

#include "modules/geometry/__/weighted_mean_matrix.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <glm/common.hpp>

namespace sight::module::geometry
{

//-----------------------------------------------------------------------------

weighted_mean_matrix::weighted_mean_matrix() :
    filter(m_signals)
{
}

//-----------------------------------------------------------------------------

void weighted_mean_matrix::configuring(const config_t& /*unused*/)
{
}

//-----------------------------------------------------------------------------

void weighted_mean_matrix::starting()
{
}

//-----------------------------------------------------------------------------

service::connections_t weighted_mean_matrix::auto_connections() const
{
    return {
        {m_matrix_in, data::object::MODIFIED_SIG, sight::service::slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

void weighted_mean_matrix::updating()
{
    auto current_mat = m_matrix_in.const_lock();

    if(!m_enabled.value())
    {
        auto output = m_matrix_out.lock();
        output->deep_copy(current_mat.get_shared());
        output->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG)->async_emit();
        this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
        m_initialized = false;
        return;
    }

    glm::dmat4 glm_matrix  = sight::geometry::data::to_glm_mat(*current_mat);
    glm::dquat orientation = glm::quat_cast(glm_matrix);
    glm::dvec3 translation = glm_matrix[3];

    if(!m_initialized)
    {
        m_current_weighted_orientation = orientation;
        m_current_weighted_translation = translation;
        m_initialized                  = true;
    }

    SIGHT_ASSERT("Weight value (" << *m_weight << ") should be > 0 and <= 1", *m_weight > 0. && *m_weight <= 1.);

    // slerp rotation
    m_current_weighted_orientation = glm::slerp(m_current_weighted_orientation, orientation, *m_weight);

    // interpolate position
    m_current_weighted_translation = glm::mix(m_current_weighted_translation, translation, *m_weight);

    // recombine to a matrix
    glm::dmat4 glm_output = glm::mat4_cast(m_current_weighted_orientation);
    glm_output[3][0] = m_current_weighted_translation[0];
    glm_output[3][1] = m_current_weighted_translation[1];
    glm_output[3][2] = m_current_weighted_translation[2];

    {
        auto output = m_matrix_out.lock();

        sight::geometry::data::from_glm_mat(*output, glm_output);
        output->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG)->async_emit();
    }

    this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
}

//-----------------------------------------------------------------------------

void weighted_mean_matrix::stopping()
{
    m_initialized = false;
}

} // namespace sight::module::geometry
