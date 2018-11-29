/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Port.hpp"

fwDataRegisterMacro( ::fwData::Port );

namespace fwData
{
//------------------------------------------------------------------------------

Port::Port(::fwData::Object::Key key) : m_identifier("IDNOTdefined"),
                                        m_type("TypeNotDefined")
{
}

//------------------------------------------------------------------------------

Port::~Port()
{
}

//------------------------------------------------------------------------------

void Port::shallowCopy(const Object::csptr &_source )
{
    Port::csptr other = Port::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_identifier = other->m_identifier;
    m_type       = other->m_type;
}

//------------------------------------------------------------------------------

void Port::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Port::csptr other = Port::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_identifier = other->m_identifier;
    m_type       = other->m_type;
}

//------------------------------------------------------------------------------

} // namespace fwData
