/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <boost/foreach.hpp>

#include "fwData/registry/macros.hpp"


#include "fwData/Composite.hpp"


fwDataRegisterMacro( ::fwData::Composite );

namespace fwData
{


Composite::Composite()
{
    SLM_TRACE_FUNC();
}


Composite::~Composite()
{
    SLM_TRACE_FUNC();
}


//------------------------------------------------------------------------------

void Composite::shallowCopy( Composite::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_attrContainer.clear();

    m_attrContainer = _source->m_attrContainer;
}

//------------------------------------------------------------------------------

void Composite::deepCopy( Composite::csptr source )
{
    this->fieldDeepCopy( source );

    m_attrContainer.clear();

    BOOST_FOREACH(const ValueType &elem, *source)
    {
        m_attrContainer.insert( ValueType(elem.first, ::fwData::Object::copy(elem.second) ) );
    }
}

//------------------------------------------------------------------------------

}
