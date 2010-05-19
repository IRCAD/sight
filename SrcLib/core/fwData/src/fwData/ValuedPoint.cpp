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

#include "fwData/ValuedPoint.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::ValuedPoint, ::fwData::ValuedPoint );

namespace fwData
{

//------------------------------------------------------------------------------

ValuedPoint::ValuedPoint ()
{
    SLM_TRACE_FUNC();
    m_value = 0.0;
}

//------------------------------------------------------------------------------

ValuedPoint::~ValuedPoint ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ValuedPoint::shallowCopy( ValuedPoint::csptr _source )
{
    ::fwData::Point::shallowCopy( ::fwData::Point::dynamicConstCast( _source ) ) ;
    this->m_value = _source->m_value;
}

//------------------------------------------------------------------------------

void ValuedPoint::deepCopy( ValuedPoint::csptr _source )
{
    ::fwData::Point::deepCopy( ::fwData::Point::dynamicConstCast( _source ) ) ;
    this->m_value = _source->m_value;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

} // namespace fwData



