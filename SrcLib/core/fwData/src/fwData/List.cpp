/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"


#include "fwData/List.hpp"


fwDataRegisterMacro( ::fwData::List );

namespace fwData
{

//------------------------------------------------------------------------------

List::List()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

List::~List()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void List::shallowCopy( List::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_attrContainer = _source->m_attrContainer;
}

//------------------------------------------------------------------------------

void List::deepCopy( List::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_attrContainer.clear();
    std::transform(
            _source->begin(), _source->end(),
            std::back_inserter(m_attrContainer),
            &::fwData::Object::copy< ValueType::element_type >
    );
}

//------------------------------------------------------------------------------

}
