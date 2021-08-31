/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SMarkerToPoint.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Matrix4.hpp>
#include <data/Point.hpp>

#include <service/IService.hpp>

namespace sight::module::geometry::vision
{

const core::com::Slots::SlotKeyType SMarkerToPoint::s_ADD_POINT_SLOT = "addPoint";
const core::com::Slots::SlotKeyType SMarkerToPoint::s_CLEAR_SLOT     = "clear";

// ----------------------------------------------------------------------------

SMarkerToPoint::SMarkerToPoint() noexcept
{
    newSlot(s_ADD_POINT_SLOT, &SMarkerToPoint::addPoint, this);
    newSlot(s_CLEAR_SLOT, &SMarkerToPoint::clear, this);
}

// ----------------------------------------------------------------------------

SMarkerToPoint::~SMarkerToPoint() noexcept
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::configuring()
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::starting()
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::updating()
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::stopping()
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::addPoint()
{
    const auto matrixTL = m_matrixTL.lock();

    data::Matrix4::sptr matrix3D = data::Matrix4::New();

    core::HiResClock::HiResClockType currentTimestamp = core::HiResClock::getTimeInMilliSec();
    CSPTR(data::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);
    SIGHT_ASSERT("Buffer not found with timestamp " << currentTimestamp, buffer);

    const float* values = buffer->getElement(0);

    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            matrix3D->setCoefficient(i, j, values[i * 4 + j]);
        }
    }

    SIGHT_DEBUG(
        "Marker Center Position : " << matrix3D->getCoefficient(0, 3) << " , "
        << matrix3D->getCoefficient(1, 3) << " , "
        << matrix3D->getCoefficient(2, 3)
    );

    //Save the position and drop the orientation
    data::Point::sptr p = data::Point::New(
        matrix3D->getCoefficient(0, 3),
        matrix3D->getCoefficient(1, 3),
        matrix3D->getCoefficient(2, 3)
    );

    const auto pl = m_pointList.lock();
    pl->pushBack(p);
    auto sig = pl->signal<data::PointList::PointAddedSignalType>(data::PointList::s_POINT_ADDED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(p);
    }
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::clear()
{
    const auto pl = m_pointList.lock();

    if(pl && !pl->getPoints().empty())
    {
        pl->clear();

        auto sig = pl->signal<data::PointList::ModifiedSignalType>(data::PointList::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

} //namespace sight::module::geometry::vision
