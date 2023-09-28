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

#include "modules/geometry/__/STransformLandmark.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/Matrix4.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::geometry
{

// -----------------------------------------------------------------------------

static const core::com::signals::key_t LANDMARK_UPDATED_SIG = "landmarkUpdated";

const core::com::slots::key_t SELECTED_POINT_SLOT = "updateSelectedPoint";
const core::com::slots::key_t UPDATE_POINT_SLOT   = "updatePoint";
const core::com::slots::key_t REMOVE_POINT_SLOT   = "removePoint";

// -----------------------------------------------------------------------------

STransformLandmark::STransformLandmark() noexcept
{
    new_signal<LandmarkUpdatedSignalType>(LANDMARK_UPDATED_SIG);
    new_slot(SELECTED_POINT_SLOT, &STransformLandmark::updateSelectedPoint, this);
    new_slot(UPDATE_POINT_SLOT, &STransformLandmark::updatePoint, this);
    new_slot(REMOVE_POINT_SLOT, &STransformLandmark::removePoint, this);
}

// -----------------------------------------------------------------------------

STransformLandmark::~STransformLandmark() noexcept =
    default;

// -----------------------------------------------------------------------------

void STransformLandmark::starting()
{
}

// -----------------------------------------------------------------------------

void STransformLandmark::stopping()
{
}

// -----------------------------------------------------------------------------

void STransformLandmark::configuring()
{
    const ConfigType configuration = this->getConfiguration();
    m_label = configuration.get<std::string>("label", m_label);
    if(!m_label.empty())
    {
        m_landmarkSelected = true;
    }
}

// -----------------------------------------------------------------------------

void STransformLandmark::updating()
{
    if(m_landmarkSelected)
    {
        const auto landmark  = m_landmarks.lock();
        const auto transform = m_transform.lock();
        try
        {
            data::Landmarks::PointType& point = landmark->getPoint(m_label, m_index);
            point[0] = (*transform)[3];
            point[1] = (*transform)[7];
            point[2] = (*transform)[11];

            //notify point is modified
            auto sig = landmark->signal<data::Landmarks::PointModifiedSigType>(
                data::Landmarks::POINT_MODIFIED_SIG
            );
            sig->async_emit(m_label, m_index);
        }
        catch(data::Exception& e)
        {
            sight::ui::dialog::message::show(
                "Transform Landmarks",
                "It is impossible to modify landmarks: "
                + std::string(e.what()),
                sight::ui::dialog::message::WARNING
            );
        }
    }
}

// -----------------------------------------------------------------------------

service::connections_t STransformLandmark::getAutoConnections() const
{
    return {{s_TRANSFORM_INPUT, data::Object::MODIFIED_SIG, service::slots::UPDATE}};
}

// -----------------------------------------------------------------------------

void STransformLandmark::updateSelectedPoint(std::string /*name*/, std::size_t index)
{
    m_landmarkSelected = true;
    m_index            = index;
    this->update();
}

// -----------------------------------------------------------------------------

void STransformLandmark::updatePoint(std::string name)
{
    m_landmarkSelected = true;
    const auto landmark    = m_landmarks.lock();
    const std::size_t size = landmark->getGroup(name).m_points.size();
    m_index = size - 1;
    this->update();
}

// -----------------------------------------------------------------------------

void STransformLandmark::removePoint()
{
    // When a point is removed, it's not selected anymore
    m_landmarkSelected = false;
}

// -----------------------------------------------------------------------------

} // namespace sight::module::geometry
