/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "arData/MarkerMap.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

namespace arData
{

fwDataRegisterMacro( ::arData::MarkerMap )

//------------------------------------------------------------------------------

MarkerMap::MarkerMap(fwData::Object::Key)
{

}

//------------------------------------------------------------------------------

MarkerMap::~MarkerMap()
{

}

//------------------------------------------------------------------------------

void MarkerMap::shallowCopy(const fwData::Object::csptr& _source)
{
    MarkerMap::csptr other = MarkerMap::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_markers = other->m_markers;

}

//------------------------------------------------------------------------------

void MarkerMap::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache)
{
    MarkerMap::csptr other = MarkerMap::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, _cache );

    m_markers = other->m_markers;
}

//------------------------------------------------------------------------------

const MarkerMap::MarkerType* MarkerMap::getMarker(const KeyType& _id) const
{
    auto it = m_markers.find(_id);
    if(it != m_markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

MarkerMap::MarkerType* MarkerMap::getMarker(const KeyType& _id)
{
    auto it = m_markers.find(_id);
    if(it != m_markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

const MarkerMap::MarkerType& MarkerMap::getMarker(size_t index) const
{
    SLM_ASSERT("Marker map is empty", !m_markers.empty());
    SLM_ASSERT("Index is bigger than map's size", index < m_markers.size() );

    auto it = m_markers.begin();
    std::advance(it, index);
    return it->second;
}

//------------------------------------------------------------------------------

MarkerMap::MarkerType& MarkerMap::getMarker(size_t index)
{
    SLM_ASSERT("Marker map is empty", !m_markers.empty());
    SLM_ASSERT("Index is bigger than map's size", index < m_markers.size() );

    auto it = m_markers.begin();
    std::advance(it, index);
    return it->second;
}

//------------------------------------------------------------------------------

size_t MarkerMap::count() const
{
    return m_markers.size();
}

//------------------------------------------------------------------------------

void MarkerMap::setMarker(const KeyType& _id, const MarkerMap::MarkerType& _marker )
{
    m_markers[_id] = _marker;
}

//------------------------------------------------------------------------------

} // namespace arData
