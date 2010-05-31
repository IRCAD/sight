/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Port.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Port, ::fwData::Port );

namespace fwData
{
//------------------------------------------------------------------------------

Port::Port() : m_identifier("IDNOTdefined") , m_type("TypeNotDefined")
{
    // TODO Auto-generated constructor stub
}

//------------------------------------------------------------------------------

Port::~Port()
{
    // TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------

void Port::shallowCopy( Port::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    this->m_identifier = _source->m_identifier;
    this->m_type = _source->m_type;
}

//------------------------------------------------------------------------------

void Port::deepCopy( Port::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_identifier = _source->m_identifier;
    this->m_type = _source->m_type;

}

//------------------------------------------------------------------------------

} // namespace fwData
