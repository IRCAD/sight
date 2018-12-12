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

#ifndef __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HPP__
#define __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HPP__

#include "fwCamp/camp/MapValueMapper.hpp"
#include "fwCamp/camp/MapMapper.hpp"
#include "fwCamp/camp/MapProperty.hpp"

namespace camp
{
namespace detail
{

template <typename A>
class MapPropertyImpl : public MapProperty
{
public:

    typedef MapProperty::ValuePair ValuePair;
    typedef MapProperty::ObjectPair ObjectPair;

    /**
     * \brief Construct the property
     *
     * \param name Name of the property
     * \param accessor Object used to access the actual C++ property
     */
    MapPropertyImpl(const std::string& name, const A& accessor);

protected:

    /**
     * \see ArrayProperty::getSize
     */
    virtual std::size_t getSize(const UserObject& object) const;

    MapProperty::ValuePair getElement(const UserObject& object, std::size_t index ) const;

    virtual void set(const UserObject& object, const Value& key, const Value& value) const;

private:

    typedef typename boost::remove_reference<typename A::DataType>::type MapType;
    typedef camp_ext::MapMapper<MapType> Mapper;
    typedef typename Mapper::ValueType ValueType;

    /**
     * \brief Retrieve a reference to the array
     *
     * \param object Owner object
     *
     * \return Reference to the underlying array
     */
    MapType& map(const UserObject& object) const;

    A m_accessor; ///< Object used to access the actual C++ property
};


} // namespace detail
}  // namespace camp

#include "fwCamp/camp/detail/MapPropertyImpl.hxx"

#endif /* __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HPP__ */
