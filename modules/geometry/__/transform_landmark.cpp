/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/geometry/__/transform_landmark.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::geometry
{

// -----------------------------------------------------------------------------

static const core::com::signals::key_t LANDMARK_UPDATED_SIG = "landmarkUpdated";

const core::com::slots::key_t SELECTED_POINT_SLOT = "updateSelectedPoint";
const core::com::slots::key_t UPDATE_POINT_SLOT   = "updatePoint";
const core::com::slots::key_t REMOVE_POINT_SLOT   = "removePoint";

// -----------------------------------------------------------------------------

transform_landmark::transform_landmark() noexcept
{
    new_signal<landmark_updated_signal_t>(LANDMARK_UPDATED_SIG);
    new_slot(SELECTED_POINT_SLOT, &transform_landmark::update_selected_point, this);
    new_slot(UPDATE_POINT_SLOT, &transform_landmark::update_point, this);
    new_slot(REMOVE_POINT_SLOT, &transform_landmark::remove_point, this);
}

// -----------------------------------------------------------------------------

transform_landmark::~transform_landmark() noexcept =
    default;

// -----------------------------------------------------------------------------

void transform_landmark::starting()
{
}

// -----------------------------------------------------------------------------

void transform_landmark::stopping()
{
}

// -----------------------------------------------------------------------------

void transform_landmark::configuring()
{
    const config_t configuration = this->get_config();
    m_label = configuration.get<std::string>("label", m_label);
    if(!m_label.empty())
    {
        m_landmark_selected = true;
    }
}

// -----------------------------------------------------------------------------

void transform_landmark::updating()
{
    if(m_landmark_selected)
    {
        const auto landmark  = m_landmarks.lock();
        const auto transform = m_transform.lock();
        try
        {
            data::landmarks::point_t& point = landmark->get_point(m_label, m_index);
            point[0] = (*transform)[3];
            point[1] = (*transform)[7];
            point[2] = (*transform)[11];

            //notify point is modified
            auto sig = landmark->signal<data::landmarks::point_modified_sig_t>(
                data::landmarks::POINT_MODIFIED_SIG
            );
            sig->async_emit(m_label, m_index);
        }
        catch(data::exception& e)
        {
            sight::ui::dialog::message::show(
                "Transform Landmarks",
                "It is impossible to modify landmarks: "
                + std::string(e.what()),
                sight::ui::dialog::message::warning
            );
        }
    }
}

// -----------------------------------------------------------------------------

service::connections_t transform_landmark::auto_connections() const
{
    return {{TRANSFORM_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE}};
}

// -----------------------------------------------------------------------------

void transform_landmark::update_selected_point(std::string /*name*/, std::size_t _index)
{
    m_landmark_selected = true;
    m_index             = _index;
    this->update();
}

// -----------------------------------------------------------------------------

void transform_landmark::update_point(std::string _name)
{
    m_landmark_selected = true;
    const auto landmark    = m_landmarks.lock();
    const std::size_t size = landmark->get_group(_name).m_points.size();
    m_index = size - 1;
    this->update();
}

// -----------------------------------------------------------------------------

void transform_landmark::remove_point()
{
    // When a point is removed, it's not selected anymore
    m_landmark_selected = false;
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
