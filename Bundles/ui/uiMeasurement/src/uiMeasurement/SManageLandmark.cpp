/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
