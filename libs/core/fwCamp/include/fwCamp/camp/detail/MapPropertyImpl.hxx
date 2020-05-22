/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HXX__
#define __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HXX__

#include "fwCamp/camp/customtype.hpp"
#include <fwCamp/Mapper/ValueMapper.hpp>

#include <camp/class.hpp>
#include <camp/classbuilder.hpp>
#include <camp/userobject.hpp>

namespace camp
{

namespace detail
{

template <typename A>
MapPropertyImpl<A>::MapPropertyImpl(const std::string& name, const A& accessor) :
    camp::MapProperty(name, camp::mapType<ValueType>())
    ,
    m_accessor(accessor)
{
}

//------------------------------------------------------------------------------

template <typename A>
std::size_t MapPropertyImpl<A>::getSize(const UserObject& object) const
{
    return Mapper::size(map(object));
}
//------------------------------------------------------------------------------

template <typename A>
void MapPropertyImpl<A>::set(const UserObject& object, const Value& key, const Value& value) const
{
    const typename Mapper::KeyType& typedKey      = key.to< typename Mapper::KeyType >();
    const typename Mapper::MappedType& typedValue = value.to< typename Mapper::MappedType >();

    Mapper::set(map(object), typedKey, typedValue );
}

//------------------------------------------------------------------------------

template <typename A>
MapProperty::ValuePair MapPropertyImpl<A>::getElement(const UserObject& object, std::size_t index ) const
{

    ValueType p(Mapper::get(map(object), index));
    return ValuePair(p.first, p.second);
}

template <typename A>
typename MapPropertyImpl<A>::MapType&
MapPropertyImpl<A>::map(const UserObject &object) const
{
    return m_accessor.get(object.get<typename A::ClassType>());
}

/*
 * Instantiate map properties
 */
template <typename A>
struct PropertyMapper<A, camp::mappingType>
{
    typedef MapPropertyImpl<A> Type;
};

} // namespace detail
} // namespace camp

#endif /* __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HXX__*/
