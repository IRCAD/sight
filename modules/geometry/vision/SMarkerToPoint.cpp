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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hxx>

#include <data/Matrix4.hpp>
#include <data/MatrixTL.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

namespace sight::modules::geometry::vision
{
fwServicesRegisterMacro( ::sight::service::IController, ::sight::modules::geometry::vision::SMarkerToPoint,
                         ::sight::data::PointList)

const core::com::Slots::SlotKeyType SMarkerToPoint::s_ADD_POINT_SLOT = "addPoint";
const core::com::Slots::SlotKeyType SMarkerToPoint::s_CLEAR_SLOT = "clear";

const service::IService::KeyType SMarkerToPoint::s_MATRIXTL_INPUT  = "matrixTL";
const service::IService::KeyType SMarkerToPoint::s_POINTLIST_INOUT = "pointList";

// ----------------------------------------------------------------------------

SMarkerToPoint::SMarkerToPoint() noexcept
{
    newSlot( s_ADD_POINT_SLOT, &SMarkerToPoint::addPoint, this );
    newSlot( s_CLEAR_SLOT, &SMarkerToPoint::clear, this );
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

void SMarkerToPoint::swapping()
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
    data::MatrixTL::csptr matrixTL = this->getInput< data::MatrixTL >(s_MATRIXTL_INPUT);
    data::PointList::sptr pl       = this->getInOut< data::PointList >(s_POINTLIST_INOUT);

    data::Matrix4::sptr matrix3D = data::Matrix4::New();

    core::HiResClock::HiResClockType currentTimestamp = core::HiResClock::getTimeInMilliSec();
    CSPTR(data::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);
    SLM_ASSERT("Buffer not found with timestamp " << currentTimestamp, buffer);

    const float* values = buffer->getElement(0);

    for(unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j = 0; j < 4; ++j)
        {
            matrix3D->setCoefficient(i, j, values[i*4+j]);
        }
    }

    SLM_DEBUG("Marker Center Position : "<< matrix3D->getCoefficient(0, 3)<<" , "
                                         <<matrix3D->getCoefficient(1, 3)<<" , "
                                         <<matrix3D->getCoefficient(2, 3));

    //Save the position and drop the orientation
    data::Point::sptr p = data::Point::New(matrix3D->getCoefficient(0, 3),
                                           matrix3D->getCoefficient(1, 3),
                                           matrix3D->getCoefficient(2, 3));

    pl->pushBack(p);
    auto sig = pl->signal< data::PointList::PointAddedSignalType >(data::PointList::s_POINT_ADDED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(p);
    }

}

// ----------------------------------------------------------------------------

void SMarkerToPoint::clear()
{
    data::PointList::sptr pl = this->getInOut< data::PointList >(s_POINTLIST_INOUT);

    data::mt::ObjectReadLock lock(pl);

    if (pl && !pl->getPoints().empty())
    {
        pl->clear();

        auto sig = pl->signal< data::PointList::ModifiedSignalType >(data::PointList::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

} //namespace sight::modules::geometry::vision
