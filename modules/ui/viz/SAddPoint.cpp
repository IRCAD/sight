/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "SAddPoint.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <geometry/data/PointList.hpp>

#include <glm/geometric.hpp>

#include <service/macros.hpp>

namespace sight::module::ui::viz
{

const core::com::Slots::SlotKeyType SAddPoint::s_PICK_SLOT         = "pick";
const core::com::Slots::SlotKeyType SAddPoint::s_CLEAR_POINTS_SLOT = "clearPoints";

//------------------------------------------------------------------------------

SAddPoint::SAddPoint() noexcept
{
    FW_DEPRECATED("::sight::module::ui::viz::SAddPoint", "::sight::module::geometry::base::SManagePointList", "21.0");

    newSlot(s_PICK_SLOT, &SAddPoint::pick, this);
    newSlot(s_CLEAR_POINTS_SLOT, &SAddPoint::clearPoints, this);
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

void SAddPoint::addPoint(const data::Point::sptr _point)
{
    auto pointList = m_pointList.lock();
    SIGHT_ASSERT("Missing data::PointList data", pointList);

    pointList->pushBack(_point);

    auto sig = pointList->signal<data::PointList::PointAddedSignalType>(data::PointList::s_POINT_ADDED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(_point);
    }
}

//------------------------------------------------------------------------------

void SAddPoint::removePoint(const data::Point::csptr _point)
{
    auto pointList = m_pointList.lock();
    SIGHT_ASSERT("Missing data::PointList data", pointList);

    const data::Point::sptr pointRes =
        geometry::data::PointList::removeClosestPoint(pointList.get_shared(), _point, 10);

    if(pointRes != nullptr)
    {
        const auto& sig = pointList->signal<data::PointList::PointRemovedSignalType>(
            data::PointList::s_POINT_REMOVED_SIG
        );
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit(pointRes);
        }
    }
}

//------------------------------------------------------------------------------

void SAddPoint::pick(data::tools::PickingInfo _info)
{
    data::Point::sptr point                = data::Point::New();
    data::Point::PointCoordArrayType cords = {{_info.m_worldPos[0], _info.m_worldPos[1], _info.m_worldPos[2]}};
    point->setCoord(cords);

    if(_info.m_modifierMask & data::tools::PickingInfo::CTRL)
    {
        if(_info.m_eventId == data::tools::PickingInfo::Event::MOUSE_LEFT_UP)
        {
            this->addPoint(point);
        }
        else if(_info.m_eventId == data::tools::PickingInfo::Event::MOUSE_RIGHT_UP)
        {
            this->removePoint(point);
        }
    }
}

//------------------------------------------------------------------------------

void SAddPoint::clearPoints()
{
    auto pointList = m_pointList.lock();
    SIGHT_ASSERT("Missing data::PointList data", pointList);

    pointList->clear();
    const auto& sig = pointList->signal<data::PointList::ModifiedSignalType>(
        data::PointList::s_MODIFIED_SIG
    );
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

} // namespace sight::module::ui::viz
