/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include <fwData/Factory.hpp>

#include "fwData/PointList.hpp"

fwDataRegisterMacro( ::fwData::PointList );

namespace fwData
{

//------------------------------------------------------------------------------

PointList::PointList ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PointList::~PointList ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PointList::shallowCopy( PointList::csptr _source )
{
    this->::fwData::Object::fieldShallowCopy( _source );

    this->m_vPoints = _source->m_vPoints;
}

//------------------------------------------------------------------------------

void PointList::deepCopy( PointList::csptr _source )
{
    this->::fwData::Object::fieldDeepCopy( _source );

    this->m_vPoints.clear();
    for (   PointList::PointListContainer::const_iterator iter = _source->m_vPoints.begin();
            iter != _source->m_vPoints.end();
            ++iter )
    {
        Point::NewSptr newPoint;
        newPoint->deepCopy( *iter );
        this->m_vPoints.push_back( newPoint );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


