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

#include "uiVisuOgre/SAddPoint.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/helper/PointList.hpp>

#include <fwServices/macros.hpp>

#include <glm/geometric.hpp>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwServices::IController, ::uiVisuOgre::SAddPoint, ::fwData::PointList);

const ::fwCom::Slots::SlotKeyType SAddPoint::s_ADD_POINT_SLOT    = "addPoint";
const ::fwCom::Slots::SlotKeyType SAddPoint::s_REMOVE_POINT_SLOT = "removePoint";
const ::fwCom::Slots::SlotKeyType SAddPoint::s_CLEAR_POINTS_SLOT = "clearPoints";

const std::string s_POINTLIST_KEY = "pointList";

//------------------------------------------------------------------------------
SAddPoint::SAddPoint() noexcept
{
    newSlot( s_ADD_POINT_SLOT, &SAddPoint::addPoint, this );
    newSlot( s_REMOVE_POINT_SLOT, &SAddPoint::removePoint, this );
    newSlot( s_CLEAR_POINTS_SLOT, &SAddPoint::clearPoints, this );
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

void SAddPoint::addPoint(::fwData::Object::sptr _pointObject)
{
    auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_KEY);
    OSLM_ASSERT("Missing ::fwData::PointList data", pointList);

    const auto point = ::fwData::Point::dynamicCast(_pointObject);
    {
        ::fwData::mt::ObjectWriteLock lock(pointList);
        pointList->pushBack(point);
    }

    auto sig = pointList->signal< ::fwData::PointList::PointAddedSignalType >(::fwData::PointList::s_POINT_ADDED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(point );
    }
}

//------------------------------------------------------------------------------

void SAddPoint::removePoint(::fwData::Object::sptr _pointObject)
{
    auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_KEY);
    OSLM_ASSERT("Missing ::fwData::PointList data", pointList);

    const auto point = ::fwData::Point::dynamicCast(_pointObject);
    ::fwData::mt::ObjectWriteLock lock(pointList);

    const ::fwData::Point::sptr pointRes = ::fwDataTools::helper::PointList::removeClosestPoint(pointList, point, 10);

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

} // namespace uiVisuOgre
