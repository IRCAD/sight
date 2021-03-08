/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/Port.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA( sight::data::Port );

namespace sight::data
{
//------------------------------------------------------------------------------

Port::Port(data::Object::Key) :
    m_identifier("IDNOTdefined"),
    m_type("TypeNotDefined")
{
}

//------------------------------------------------------------------------------

Port::~Port()
{
}

//------------------------------------------------------------------------------

void Port::shallowCopy(const Object::csptr& _source )
{
    Port::csptr other = Port::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_identifier = other->m_identifier;
    m_type       = other->m_type;
}

//------------------------------------------------------------------------------

void Port::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Port::csptr other = Port::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_identifier = other->m_identifier;
    m_type       = other->m_type;
}

//------------------------------------------------------------------------------

} // namespace sight::data
