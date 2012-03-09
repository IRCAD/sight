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
{}

//------------------------------------------------------------------------------

Port::~Port()
{}

//------------------------------------------------------------------------------

void Port::shallowCopy( Port::csptr _source )
{
    this->fieldShallowCopy( _source );

    m_identifier = _source->m_identifier;
    m_type = _source->m_type;
}

//------------------------------------------------------------------------------

void Port::deepCopy( Port::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_identifier = _source->m_identifier;
    m_type = _source->m_type;
}

//------------------------------------------------------------------------------

} // namespace fwData
