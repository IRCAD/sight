/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/ValuedPointList.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::ValuedPointList, ::fwData::ValuedPointList );

namespace fwData
{

//------------------------------------------------------------------------------

ValuedPointList::ValuedPointList ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ValuedPointList::~ValuedPointList ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ValuedPointList::shallowCopy( ValuedPointList::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    this->m_vPoints = _source->m_vPoints;
}

//------------------------------------------------------------------------------

void ValuedPointList::deepCopy( ValuedPointList::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_vPoints.clear();
    for (   ValuedPointList::PointListContainer::const_iterator iter = _source->m_vPoints.begin();
            iter != _source->m_vPoints.end();
            ++iter )
    {
        ValuedPoint::NewSptr newPoint;
        newPoint->deepCopy( *iter );
        this->m_vPoints.push_back( newPoint );
    }
}

//------------------------------------------------------------------------------

} // namespace fwData


