/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SImagePicker.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

namespace videoTools
{

fwServicesRegisterMacro( ::fwServices::IController, ::videoTools::SImagePicker, ::fwData::PointList);

//-----------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

SImagePicker::SImagePicker() noexcept
{
    newSlot(s_GET_INTERACTION_SLOT, &SImagePicker::getInteraction, this);
}

//-----------------------------------------------------------------------------

SImagePicker::~SImagePicker() noexcept
{
}

//-----------------------------------------------------------------------------

void SImagePicker::starting()
{
    m_pointList = this->getInOut< ::fwData::PointList >("pointList");
}

//-----------------------------------------------------------------------------

void SImagePicker::stopping()
{
}

//-----------------------------------------------------------------------------

void SImagePicker::configuring()
{
}

//-----------------------------------------------------------------------------

void SImagePicker::updating()
{
}

//-----------------------------------------------------------------------------

void SImagePicker::getInteraction(::fwDataTools::PickingInfo info)
{
    if (info.m_modifierMask == ::fwDataTools::PickingInfo::CTRL)
    {
        const double x = info.m_worldPos[0];
        const double y = -(info.m_worldPos[1]);
        const double z = info.m_worldPos[2];

        std::array<double, 3> position = {{x, y, z}};

        if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_DOWN)
        {
            this->addPoint(position);
        }
        else if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_DOWN)
        {
            this->removeLastPoint();
        }

    }
}

//-----------------------------------------------------------------------------

void SImagePicker::addPoint(const std::array<double, 3>& currentPoint )
{
    ::fwData::Point::sptr point = ::fwData::Point::New(currentPoint[0], currentPoint[1], 0.);

    m_pointList->getPoints().push_back(point);
    auto sig = m_pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImagePicker::removeLastPoint()
{
    if (!m_pointList->getPoints().empty())
    {
        m_pointList->getPoints().pop_back();
    }
    auto sig = m_pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

} // namespace uiVideo
