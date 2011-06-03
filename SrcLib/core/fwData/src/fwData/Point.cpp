/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/Point.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Point, ::fwData::Point );

namespace fwData
{

//------------------------------------------------------------------------------

Point::Point ()
{
    SLM_TRACE_FUNC();
    m_vCoord[0] = 0.0;
    m_vCoord[1] = 0.0;
    m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

Point::sptr Point::PointFactory(float x, float y, float z)
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = x;
    point->m_vCoord[1] = y;
    point->m_vCoord[2] = z;
    return point;
}

//------------------------------------------------------------------------------

Point::sptr Point::PointFactory( Point::sptr p )
{
    Point::sptr point = ::fwData::Point::New();
    point->m_vCoord[0] = p->m_vCoord[0];
    point->m_vCoord[1] = p->m_vCoord[1];
    point->m_vCoord[2] = p->m_vCoord[2];
    return point;
}

//------------------------------------------------------------------------------

Point::~Point ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Point::shallowCopy( Point::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_vCoord = _source->m_vCoord;
}

//------------------------------------------------------------------------------

void Point::deepCopy( Point::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_vCoord = _source->m_vCoord;
}

//------------------------------------------------------------------------------

} // namespace fwData


