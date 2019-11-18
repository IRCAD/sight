/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "opPicking/SAddPoint.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/helper/PointList.hpp>

#include <fwServices/macros.hpp>

#include <glm/geometric.hpp>

namespace opPicking
{

static const ::fwCom::Slots::SlotKeyType s_PICK_SLOT         = "pick";
static const ::fwCom::Slots::SlotKeyType s_CLEAR_POINTS_SLOT = "clearPoints";

static const std::string s_POINTLIST_INOUT = "pointList";

static const std::string s_MAX_CONFIG       = "max";
static const std::string s_REMOVABLE_CONFIG = "removable";

fwServicesRegisterMacro( ::fwServices::IController, ::opPicking::SAddPoint, ::fwData::PointList)

//------------------------------------------------------------------------------

SAddPoint::SAddPoint() noexcept
{
    newSlot(s_PICK_SLOT, &SAddPoint::pick, this );
    newSlot(s_CLEAR_POINTS_SLOT, &SAddPoint::clearPoints, this );
}

//------------------------------------------------------------------------------

SAddPoint::~SAddPoint() noexcept
{
}

//------------------------------------------------------------------------------

void SAddPoint::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_max       = config.get<size_t>(s_MAX_CONFIG, m_max);
    m_removable = config.get<bool>(s_REMOVABLE_CONFIG, m_removable);
}

//------------------------------------------------------------------------------

void SAddPoint::starting()
{
}

//------------------------------------------------------------------------------

void SAddPoint::updating()
{
}

//------------------------------------------------------------------------------

void SAddPoint::stopping()
{
}

//------------------------------------------------------------------------------

void SAddPoint::addPoint(const ::fwData::Point::sptr _point) const
{
    const auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
    SLM_ASSERT("'" + s_POINTLIST_INOUT + "' does not exist.", pointList);

    {
        const ::fwData::mt::ObjectWriteLock lock(pointList);
        pointList->pushBack(_point);
        if(m_max != 0 && pointList->getPoints().size() > m_max)
        {
            const ::fwData::Point::sptr removedPoint = pointList->getPoints()[0];
            pointList->remove(0);
            const auto& sig = pointList->signal< ::fwData::PointList::PointRemovedSignalType >(
                ::fwData::PointList::s_POINT_REMOVED_SIG);
            sig->asyncEmit(removedPoint);
        }
    }

    const auto& sig = pointList->signal< ::fwData::PointList::PointAddedSignalType >(
        ::fwData::PointList::s_POINT_ADDED_SIG);
    sig->asyncEmit(_point);
}

//------------------------------------------------------------------------------

void SAddPoint::removePoint(const ::fwData::Point::csptr _point) const
{
    if(m_removable)
    {
        const auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
        SLM_ASSERT("'" + s_POINTLIST_INOUT + "' does not exist.", pointList);

        const ::fwData::mt::ObjectWriteLock lock(pointList);

        const ::fwData::Point::sptr pointRes =
            ::fwDataTools::helper::PointList::removeClosestPoint(pointList, _point, 10);

        if(pointRes != nullptr)
        {
            const auto& sig = pointList->signal< ::fwData::PointList::PointRemovedSignalType >(
                ::fwData::PointList::s_POINT_REMOVED_SIG);
            sig->asyncEmit(pointRes);
        }
    }
}

//------------------------------------------------------------------------------

void SAddPoint::pick(::fwDataTools::PickingInfo _info) const
{
    ::fwData::Point::sptr point = ::fwData::Point::New();
    const ::fwData::Point::PointCoordArrayType cords =
    {{_info.m_worldPos[0], _info.m_worldPos[1], _info.m_worldPos[2]}};
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

void SAddPoint::clearPoints() const
{
    const auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
    SLM_ASSERT("'" + s_POINTLIST_INOUT + "' does not exist.", pointList);

    const ::fwData::mt::ObjectWriteLock lock(pointList);

    pointList->clear();
    const auto& sig = pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG);
    {
        sig->asyncEmit();
    }
}

} // namespace opPicking
