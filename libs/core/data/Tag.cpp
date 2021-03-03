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

#include "data/Tag.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA( sight::data::Tag );

namespace sight::data
{
//------------------------------------------------------------------------------

Tag::Tag(data::Object::Key) :
    m_size(0.5)
{
}

//------------------------------------------------------------------------------

Tag::~Tag ()
{
    if(m_pointList)
    {
        m_pointList->getPoints().clear();
    }
}

//------------------------------------------------------------------------------

void Tag::shallowCopy(const Object::csptr& source )
{
    Tag::csptr other = Tag::dynamicConstCast(source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( source );
    m_sType     = other->m_sType;
    m_size      = other->m_size;
    m_pointList = other->m_pointList;
}

//------------------------------------------------------------------------------

void Tag::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    Tag::csptr other = Tag::dynamicConstCast(source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );
    m_sType     = other->m_sType;
    m_size      = other->m_size;
    m_pointList = data::Object::copy(other->m_pointList, cache);
}

} // namespace sight::data
