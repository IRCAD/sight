/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "maths/STransformLandmark.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Exception.hpp>
#include <data/Landmarks.hpp>
#include <data/tools/TransformationMatrix3D.hpp>

#include <services/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

namespace maths
{

// -----------------------------------------------------------------------------

const services::IService::KeyType s_LANDMARK_INOUT                  = "landmark";
const services::IService::KeyType s_TRANSFORM_INPUT                 = "matrix";
static const core::com::Signals::SignalKeyType LANDMARK_UPDATED_SIG = "landmarkUpdated";

const core::com::Slots::SlotKeyType s_SELECTED_POINT_SLOT = "updateSelectedPoint";
const core::com::Slots::SlotKeyType s_UPDATE_POINT_SLOT   = "updatePoint";
const core::com::Slots::SlotKeyType s_REMOVE_POINT_SLOT   = "removePoint";

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::services::IService, ::maths::STransformLandmark )

// -----------------------------------------------------------------------------

STransformLandmark::STransformLandmark() noexcept
{
    newSignal< LandmarkUpdatedSignalType>(LANDMARK_UPDATED_SIG);
    newSlot(s_SELECTED_POINT_SLOT, &STransformLandmark::updateSelectedPoint, this);
    newSlot(s_UPDATE_POINT_SLOT, &STransformLandmark::updatePoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &STransformLandmark::removePoint, this);
}

// -----------------------------------------------------------------------------

STransformLandmark::~STransformLandmark() noexcept
{
}

// -----------------------------------------------------------------------------

void STransformLandmark::starting()
{
    const auto transformLocked = this->getLockedInput< data::TransformationMatrix3D>(s_TRANSFORM_INPUT);
    m_transform = transformLocked.get_shared();
}

// -----------------------------------------------------------------------------

void STransformLandmark::stopping()
{
}

// -----------------------------------------------------------------------------

void STransformLandmark::configuring()
{
    const ConfigType configuration = this->getConfigTree();
    m_label = configuration.get< std::string >("label", m_label);
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
        const auto landmark = this->getLockedInOut< data::Landmarks >(s_LANDMARK_INOUT);
        try
        {
            data::Landmarks::PointType& point               = landmark->getPoint(m_label, m_index);
            data::TransformationMatrix3D::TMCoefArray array = m_transform->getCoefficients();
            point[0] = array[3];
            point[1] = array[7];
            point[2] = array[11];

            //notify point is modified
            auto sig = landmark->signal< data::Landmarks::PointModifiedSigType >(
                data::Landmarks::s_POINT_MODIFIED_SIG);
            sig->asyncEmit(m_label, m_index);
        }
        catch (data::Exception& e )
        {
            ui::base::dialog::MessageDialog::showMessageDialog("Transform Landmarks",
                                                               "It is impossible to modify landmarks: "
                                                               + std::string(e.what()),
                                                               ui::base::dialog::IMessageDialog::WARNING);
        }
    }
}

// -----------------------------------------------------------------------------

::services::IService::KeyConnectionsMap STransformLandmark::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// -----------------------------------------------------------------------------

void STransformLandmark::updateSelectedPoint(std::string name, size_t index)
{
    m_landmarkSelected = true;
    m_index            = index;
    this->update();

}
// -----------------------------------------------------------------------------

void STransformLandmark::updatePoint(std::string name)
{
    m_landmarkSelected = true;
    const auto landmark = this->getLockedInOut< data::Landmarks >(s_LANDMARK_INOUT);
    const size_t size   = landmark->getGroup(name).m_points.size();
    m_index = size -1;
    this->update();
}

// -----------------------------------------------------------------------------

void STransformLandmark::removePoint()
{
    // When a point is removed, it's not selected anymore
    m_landmarkSelected = false;
}

// -----------------------------------------------------------------------------

} // maths
