/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#pragma once

#include "data/timeline/generic_object.hpp"

namespace sight::data::timeline
{

//-----------------------------------------------------------------------------

template<typename TYPE>
generic_object<TYPE>::generic_object(
    unsigned int _m_max_element_num,
    core::clock::type _timestamp,
    buffer_data_t _buffer, // NOLINT(readability-non-const-parameter)
    std::size_t _size,
    deleter_t _d
) :
    generic_object_base(_m_max_element_num, _timestamp, _buffer, _size, _d)
{
    // init all floating point values to 0
    memset(data(), 0, _m_max_element_num * get_element_size());
}

//-----------------------------------------------------------------------------

template<typename TYPE> generic_object<TYPE>::~generic_object()
= default;

//-----------------------------------------------------------------------------

template<typename TYPE>
const TYPE& generic_object<TYPE>::get_element(unsigned int _index) const
{
    SIGHT_ASSERT("Index out of bounds", _index < max_element_num());

    return *(reinterpret_cast<const element_t*>(data() + _index * get_element_size()));
}

//-----------------------------------------------------------------------------

template<typename TYPE>
void generic_object<TYPE>::set_element(const element_t& _element, unsigned int _index)
{
    static_assert(
        std::is_trivially_copyable_v<TYPE>,
        "TYPE must be TriviallyCopyable for generic_object<TYPE>::setElement to be used."
    );
    SIGHT_ASSERT("Index out of bounds", _index < max_element_num());

    // store floating point values
    auto* dst_element = reinterpret_cast<TYPE*>(data() + _index * get_element_size());
    // undefined behavior if TYPE isn't TriviallyCopyable:
    memcpy(static_cast<void*>(dst_element), &_element, get_element_size());

    // update presence mask
    set_present(_index);
}

//-----------------------------------------------------------------------------

template<typename TYPE>
TYPE* generic_object<TYPE>::add_element(unsigned int _index)
{
    SIGHT_ASSERT("Index out of bounds", _index < max_element_num());

    // update presence mask
    set_present(_index);

    // return pointer on the new element
    auto* dst_element = reinterpret_cast<TYPE*>(data() + _index * get_element_size());

    return dst_element;
}

//-----------------------------------------------------------------------------

template<typename TYPE>
generic_object<TYPE>::iterator generic_object<TYPE>::get_presence_iterator() const
{
    return iterator(*this);
}

//-----------------------------------------------------------------------------

template<typename TYPE>
generic_object<TYPE>::iterator::iterator(const generic_object_base& _object) :
    m_object(&_object),
    m_max_element(m_object->max_element_num())
{
    while(m_current_index < m_max_element && !m_object->is_present(m_current_index))
    {
        ++m_current_index;
    }
}

//-----------------------------------------------------------------------------

template<typename TYPE>
void generic_object<TYPE>::iterator::operator++()
{
    SIGHT_ASSERT("Iterator out of bounds", m_current_index < m_max_element);
    ++m_current_index;
    while(m_current_index < m_max_element && !m_object->is_present(m_current_index))
    {
        ++m_current_index;
    }
}

//-----------------------------------------------------------------------------

template<typename TYPE>
const TYPE& generic_object<TYPE>::iterator::operator*() const
{
    const auto* object = static_cast<const generic_object<element_t>*>(m_object);
    return object->get_element(m_current_index);
}

//-----------------------------------------------------------------------------

} // namespace sight::data::timeline
