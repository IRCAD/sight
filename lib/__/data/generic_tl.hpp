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

#include <data/buffer_tl.hpp>
#include <data/exception.hpp>
#include <data/timeline/generic_object.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a timeline of template objects. All objects have the same maximum number of elements,
 *          nevertheless it is not mandatory to provide all elements.
 */
template<class BUFFER_TYPE>
class generic_tl : public buffer_tl
{
public:

    SIGHT_DECLARE_CLASS(generic_tl<BUFFER_TYPE>, buffer_tl);

    using buffer_t = timeline::generic_object<BUFFER_TYPE>;

    generic_tl();
    ~generic_tl() override = default;

    /**
     * @brief Returns the closest buffer to the given timestamp
     * @param _timestamp timestamp used to find the closest buffer
     * @param _direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    sight::csptr<buffer_t> get_closest_buffer(
        core::clock::type _timestamp,
        timeline::direction_t _direction = timeline::both
    ) const;

    /// Returns the buffer matching the specified timestamp, returns NULL if object is not found
    sight::csptr<buffer_t> get_buffer(core::clock::type _timestamp) const;

    /// Initializes the size of the pool buffer.
    virtual void init_pool_size(unsigned int _max_element_num);

    /**
     * @brief Returns a new timeline::object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    sight::sptr<timeline::object> create_object(core::clock::type _timestamp) override;

    /**
     * @brief Returns a new buffer_t with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    sight::sptr<buffer_t> create_buffer(core::clock::type _timestamp);

    /// Check if the type of an object is compatible with this timeline
    bool is_object_valid(const sight::csptr<timeline::object>& _obj) const override;

    /// Get/set the maximum number of objects inside a single buffer
    unsigned int max_element_num() const;

    /// Equality comparison operators
    /// @{
    bool operator==(const generic_tl& _other) const noexcept;
    bool operator!=(const generic_tl& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Set the maximum number of objects inside a single buffer
    void set_max_element_num(unsigned int _max_element_num);

private:

    /// maximum number of elements inside a single buffer
    unsigned int m_max_element_num {~0U};
}; // class generic_tl

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
generic_tl<BUFFER_TYPE>::generic_tl() :
    buffer_tl()
{
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::shallow_copy(const object::csptr& /*_source*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const generic_tl<BUFFER_TYPE> >(_source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clear_timeline();
    this->init_pool_size(other->max_element_num());

    for(const auto& elt : other->m_timeline)
    {
        sight::sptr<buffer_t> tl_obj = this->create_buffer(elt.first);
        tl_obj->deep_copy(*elt.second);
        m_timeline.insert(timeline_t::value_type(elt.first, tl_obj));
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
sight::csptr<const typename generic_tl<BUFFER_TYPE>::buffer_t> generic_tl<BUFFER_TYPE>::get_closest_buffer(
    core::clock::type _timestamp,
    timeline::direction_t _direction
) const
{
    sight::csptr<data::timeline::object> buffer = this->get_closest_object(_timestamp, _direction);
    return std::dynamic_pointer_cast<const buffer_t>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
sight::csptr<const typename generic_tl<BUFFER_TYPE>::buffer_t> generic_tl<BUFFER_TYPE>::get_buffer(
    core::clock::type _timestamp
) const
{
    sight::csptr<data::timeline::object> buffer = this->get_object(_timestamp);
    return std::dynamic_pointer_cast<const buffer_t>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::init_pool_size(unsigned int _max_element_num)
{
    m_max_element_num = _max_element_num;
    this->alloc_pool_size(sizeof(BUFFER_TYPE) * m_max_element_num);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
sight::sptr<timeline::object> generic_tl<BUFFER_TYPE>::create_object(core::clock::type _timestamp)
{
    return this->create_buffer(_timestamp);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
sight::sptr<typename generic_tl<BUFFER_TYPE>::buffer_t> generic_tl<BUFFER_TYPE>::create_buffer(
    core::clock::type _timestamp
)
{
    sight::sptr<buffer_t> obj = std::make_shared<buffer_t>(
        m_max_element_num,
        _timestamp,
        static_cast<data::timeline::buffer::buffer_data_t>(m_pool->malloc()),
        m_pool->get_requested_size(),
        [object_ptr = m_pool](auto&& _p_h1, auto&& ...){object_ptr->free(std::forward<decltype(_p_h1)>(_p_h1));});
    return obj;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::is_object_valid(const sight::csptr<data::timeline::object>& _obj) const
{
    sight::csptr<buffer_t> src_obj = std::dynamic_pointer_cast<const buffer_t>(_obj);
    return src_obj != nullptr;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
unsigned int generic_tl<BUFFER_TYPE>::max_element_num() const
{
    return m_max_element_num;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
inline void generic_tl<BUFFER_TYPE>::set_max_element_num(unsigned int _max_element_num)
{
    m_max_element_num = _max_element_num;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::operator==(const generic_tl& _other) const noexcept
{
    if(m_max_element_num != _other.m_max_element_num)
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::operator!=(const generic_tl& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
