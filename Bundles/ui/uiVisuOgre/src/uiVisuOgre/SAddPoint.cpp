/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SAddPoint.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwServices::IController, ::uiVisuOgre::SAddPoint, ::fwData::PointList);

const ::fwCom::Slots::SlotKeyType SAddPoint::s_ADDPOINT_SLOT = "addPoint";

//------------------------------------------------------------------------------
SAddPoint::SAddPoint() noexcept
{
    newSlot( s_ADDPOINT_SLOT, &SAddPoint::addPoint, this );
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
    auto pointList = this->getObject< ::fwData::PointList >();
    OSLM_ASSERT("Missing ::fwData::PointList data", pointList);

    {
        ::fwData::mt::ObjectWriteLock lock(pointList);
        auto point = ::fwData::Point::dynamicCast(_pointObject);
        pointList->getPoints().push_back(point);
    }

    auto sig = pointList->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit( );
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
