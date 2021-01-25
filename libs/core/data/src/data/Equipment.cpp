/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/Equipment.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

fwDataRegisterMacro( ::sight::data::Equipment );

namespace sight::data
{

Equipment::Equipment(data::Object::Key)
{
}

//------------------------------------------------------------------------------

Equipment::~Equipment()
{
}

//------------------------------------------------------------------------------

void Equipment::shallowCopy(const data::Object::csptr& _source)
{
    Equipment::csptr other = Equipment::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( _source );
    m_institutionName = other->m_institutionName;
}

//------------------------------------------------------------------------------

void Equipment::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    Equipment::csptr other = Equipment::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( _source, cache );
    m_institutionName = other->m_institutionName;
}

//------------------------------------------------------------------------------

} // namespace sight::data
