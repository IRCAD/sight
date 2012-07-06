/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/PointList.hpp"

fwDataRegisterMacro( ::fwData::PointList );

namespace fwData
{

//------------------------------------------------------------------------------

PointList::PointList ()
{}

//------------------------------------------------------------------------------

PointList::~PointList ()
{}

//------------------------------------------------------------------------------

void PointList::shallowCopy( PointList::csptr _source )
{
    this->fieldShallowCopy( _source );

    m_vPoints = _source->m_vPoints;
}

//------------------------------------------------------------------------------

void PointList::deepCopy( PointList::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_vPoints.clear();
    BOOST_FOREACH(PointListContainer::value_type point, _source->m_vPoints )
    {
        Point::NewSptr newPoint;
        newPoint->deepCopy( point );
        m_vPoints.push_back( newPoint );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


