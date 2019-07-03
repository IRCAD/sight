/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "uiVisuOgre/SAddPoint.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/PointList.hpp>

#include <fwServices/macros.hpp>

#include <glm/geometric.hpp>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwServices::IController, ::uiVisuOgre::SAddPoint, ::fwData::PointList);

const ::fwCom::Slots::SlotKeyType SAddPoint::s_PICK_SLOT         = "pick";
const ::fwCom::Slots::SlotKeyType SAddPoint::s_CLEAR_POINTS_SLOT = "clearPoints";

const std::string s_POINTLIST_KEY = "pointList";

//------------------------------------------------------------------------------
SAddPoint::SAddPoint() noexcept
{
    newSlot(s_PICK_SLOT, &SAddPoint::pick, this );
    newSlot(s_CLEAR_POINTS_SLOT, &SAddPoint::clearPoints, this );
    newSlot("addPoint", &SAddPoint::addPointDeprecated, this );
    newSlot("removePoint", &SAddPoint::removePointDeprecated, this );
}

//------------------------------------------------------------------------------

SAddPoint::~SAddPoint() noexcept
{
}

//------------------------------------------------------------------------------

void SAddPoint::starting()
{
}

//------------------------------------------------------------------------------

void SAddPoint::stopping()
{
}

//------------------------------------------------------------------------------

void SAddPoint::configuring()
{
}

//------------------------------------------------------------------------------

void SAddPoint::updating()
{
}

//------------------------------------------------------------------------------

void SAddPoint::addPoint(const ::fwData::Point::sptr _point)
{
    auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_KEY);
    OSLM_ASSERT("Missing ::fwData::PointList data", pointList);

    {
        ::fwData::mt::ObjectWriteLock lock(pointList);
        pointList->pushBack(_point);
    }

    auto sig = pointList->signal< ::fwData::PointList::PointAddedSignalType >(::fwData::PointList::s_POINT_ADDED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(_point);
    }
}

//------------------------------------------------------------------------------

void SAddPoint::removePoint(const ::fwData::Point::csptr _point)
{
    auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_KEY);
    OSLM_ASSERT("Missing ::fwData::PointList data", pointList);

    ::fwData::mt::ObjectWriteLock lock(pointList);

    const ::fwData::Point::sptr pointRes = ::fwDataTools::helper::PointList::removeClosestPoint(pointList, _point, 10);

    if(pointRes != nullptr)
    {
        const auto& sig = pointList->signal< ::fwData::PointList::PointRemovedSignalType >(
            ::fwData::PointList::s_POINT_REMOVED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit(pointRes);
        }
    }
}

//------------------------------------------------------------------------------

void SAddPoint::pick(::fwDataTools::PickingInfo _info)
{
    ::fwData::Point::sptr point                = ::fwData::Point::New();
    ::fwData::Point::PointCoordArrayType cords = {{_info.m_worldPos[0], _info.m_worldPos[1], _info.m_worldPos[2]}};
    point->setCoord(cords);

    if(_info.m_modifierMask & ::fwDataTools::PickingInfo::CTRL)
    {
        if(_info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP)
        {
            this->addPoint(point);
        }
        else if(_info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_UP)
        {
            this->removePoint(point);
        }
    }
}

//------------------------------------------------------------------------------

void SAddPoint::clearPoints()
{
    auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_KEY);
    OSLM_ASSERT("Missing ::fwData::PointList data", pointList);

    ::fwData::mt::ObjectWriteLock lock(pointList);

    pointList->clear();
    const auto& sig = pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SAddPoint::addPointDeprecated(::fwData::Object::sptr _pointObject)
{
    FW_DEPRECATED_MSG("This slot is deprecated method. You should use `SAddPoint::pick(::fwDataTools::PickingInfo)`",
                      "20.0");
    const auto point = ::fwData::Point::dynamicCast(_pointObject);
    this->addPoint(point);
}

//------------------------------------------------------------------------------

void SAddPoint::removePointDeprecated(::fwData::Object::sptr _pointObject)
{
    FW_DEPRECATED_MSG("This slot is deprecated method. You should use `SAddPoint::pick(::fwDataTools::PickingInfo)`",
                      "20.0");
    const auto point = ::fwData::Point::dynamicCast(_pointObject);
    this->removePoint(point);
}

} // namespace uiVisuOgre
