/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "data/MarkerMap.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::MarkerMap)

//------------------------------------------------------------------------------

void MarkerMap::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const MarkerMap>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_markers = other->m_markers;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void MarkerMap::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const MarkerMap>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_markers = other->m_markers;

    base_class::deep_copy(other, cache);
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

const MarkerMap::MarkerType& MarkerMap::getMarker(std::size_t index) const
{
    SIGHT_ASSERT("Marker map is empty", !m_markers.empty());
    SIGHT_ASSERT("Index is bigger than map's size", index < m_markers.size());

    auto it = m_markers.begin();
    std::advance(it, static_cast<ContainerType::iterator::difference_type>(index));
    return it->second;
}

//------------------------------------------------------------------------------

MarkerMap::MarkerType& MarkerMap::getMarker(std::size_t index)
{
    SIGHT_ASSERT("Marker map is empty", !m_markers.empty());
    SIGHT_ASSERT("Index is bigger than map's size", index < m_markers.size());

    auto it = m_markers.begin();
    std::advance(it, static_cast<ContainerType::iterator::difference_type>(index));
    return it->second;
}

//------------------------------------------------------------------------------

std::size_t MarkerMap::count() const
{
    return m_markers.size();
}

//------------------------------------------------------------------------------

void MarkerMap::setMarker(const KeyType& _id, const MarkerMap::MarkerType& _marker)
{
    m_markers[_id] = _marker;
}

//------------------------------------------------------------------------------

bool MarkerMap::operator==(const MarkerMap& other) const noexcept
{
    if(!core::tools::is_equal(m_markers, other.m_markers))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool MarkerMap::operator!=(const MarkerMap& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
