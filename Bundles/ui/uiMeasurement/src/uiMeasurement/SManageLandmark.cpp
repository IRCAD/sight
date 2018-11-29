/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "uiMeasurement/SManageLandmark.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::uiMeasurement::SManageLandmark, ::fwData::PointList);

namespace uiMeasurement
{
const ::fwServices::IService::KeyType s_LANDMARKS_INOUT = "landmarks";

const ::fwCom::Slots::SlotKeyType SManageLandmark::s_CREATE_LANDMARK_SLOT = "createLandmark";
const ::fwCom::Slots::SlotKeyType SManageLandmark::s_CLEAR_SLOT           = "clearPointlist";

// ----------------------------------------------------------------------------

SManageLandmark::SManageLandmark() noexcept :
    m_counter(0)
{
    newSlot(s_CREATE_LANDMARK_SLOT, &SManageLandmark::createLandmark, this);
    newSlot(s_CLEAR_SLOT, &SManageLandmark::clearPointlist, this);
}

// ----------------------------------------------------------------------------

void SManageLandmark::configuring()
{
}

// ----------------------------------------------------------------------------

void SManageLandmark::starting()
{
}

// ----------------------------------------------------------------------------

void SManageLandmark::stopping()
{
}

// ----------------------------------------------------------------------------

void SManageLandmark::updating()
{
}

// ----------------------------------------------------------------------------

void SManageLandmark::createLandmark(::fwDataTools::PickingInfo info)
{
    if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP &&
        info.m_modifierMask == ::fwDataTools::PickingInfo::CTRL )
    {
        auto pointList = this->getInOut< ::fwData::PointList >(s_LANDMARKS_INOUT);

        m_counter = pointList->getPoints().size();

        ::fwData::Point::sptr point = ::fwData::Point::New(info.m_worldPos[0], info.m_worldPos[1], info.m_worldPos[2]);

        ::fwData::String::sptr label = ::fwData::String::New(std::to_string(m_counter));
        point->setField( ::fwDataTools::fieldHelper::Image::m_labelId, label );

        pointList->getPoints().push_back(point);

        auto sig = pointList->signal< ::fwData::PointList::PointAddedSignalType >(
            ::fwData::PointList::s_POINT_ADDED_SIG);
        sig->asyncEmit(point);
    }
}

//------------------------------------------------------------------------------

void SManageLandmark::clearPointlist()
{
    auto pointList = this->getInOut< ::fwData::PointList >(s_LANDMARKS_INOUT);

    if (pointList)
    {
        pointList->getPoints().clear();
        ::fwData::Point::sptr point;
        auto sig = pointList->signal< ::fwData::PointList::PointAddedSignalType >(
            ::fwData::PointList::s_POINT_REMOVED_SIG);
        sig->asyncEmit(point);
    }
}

// ----------------------------------------------------------------------------

}  // namespace uiMeasurement
