/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Port.hpp"

fwDataRegisterMacro( ::fwData::Port );

namespace fwData
{
//------------------------------------------------------------------------------

Port::Port() : m_identifier("IDNOTdefined") , m_type("TypeNotDefined")
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Port::~Port()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Port::shallowCopy( Port::csptr _source )
{
    this->::fwData::Object::fieldShallowCopy( _source );

    this->m_identifier = _source->m_identifier;
    this->m_type = _source->m_type;
}

//------------------------------------------------------------------------------

void Port::deepCopy( Port::csptr _source )
{
    this->::fwData::Object::fieldDeepCopy( _source );

    this->m_identifier = _source->m_identifier;
    this->m_type = _source->m_type;

}

//------------------------------------------------------------------------------

} // namespace fwData
