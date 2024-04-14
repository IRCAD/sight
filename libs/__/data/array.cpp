/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/array.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::array);

//------------------------------------------------------------------------------

inline static std::size_t compute_size(
    std::size_t _element_size,
    const data::array::size_t& _size
)
{
    std::size_t total = 0;
    if(!_size.empty())
    {
        total  = _element_size;
        total *=
            std::accumulate(
                _size.begin(),
                _size.end(),
                static_cast<std::size_t>(1),
                std::multiplies<>()
            );
    }

    return total;
}

//------------------------------------------------------------------------------

data::array::offset_t array::compute_strides(size_t _size, std::size_t _size_of_type)
{
    data::array::offset_t strides;
    strides.reserve(_size.size());

    std::size_t current_stride = _size_of_type;
    for(const auto& s : _size)
    {
        strides.push_back(current_stride);
        current_stride *= s;
    }

    return strides;
}

//------------------------------------------------------------------------------

array::array() :
    m_buffer_object(std::make_shared<core::memory::buffer_object>())
{
}

//------------------------------------------------------------------------------

array::~array()
{
    this->clear();
}

//------------------------------------------------------------------------------

void array::swap(array::sptr _source) noexcept
{
    m_fields.swap(_source->m_fields);
    m_strides.swap(_source->m_strides);
    m_size.swap(_source->m_size);
    m_buffer_object->swap(_source->m_buffer_object);

    std::swap(m_type, _source->m_type);
    std::swap(m_is_buffer_owner, _source->m_is_buffer_owner);
}

//------------------------------------------------------------------------------

void array::shallow_copy(const object::csptr& /*unused*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void array::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const array>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    if(!other->m_buffer_object->is_empty())
    {
        resize(other->m_size, other->m_type, true);
        std::memcpy(m_buffer_object->buffer(), other->m_buffer_object->buffer(), other->size_in_bytes());
    }
    else
    {
        this->clear();

        m_strides = other->m_strides;
        m_type    = other->m_type;
        m_size    = other->m_size;
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

std::size_t array::resize(
    const size_t& _size,
    const core::type& _type,
    bool _reallocate
)
{
    const std::size_t buf_size = compute_size(_type.size(), _size);

    if(_reallocate && buf_size != m_buffer_object->size())
    {
        if(m_buffer_object->is_empty())
        {
            m_is_buffer_owner = true;
            m_buffer_object->allocate(buf_size);
        }
        else if(m_is_buffer_owner)
        {
            m_buffer_object->reallocate(buf_size);
        }
        else
        {
            SIGHT_THROW_EXCEPTION_MSG(
                data::exception,
                "Tried to reallocate a not-owned Buffer."
            );
        }
    }

    m_strides = compute_strides(_size, _type.size());
    m_type    = _type;
    m_size    = _size;

    return buf_size;
}

//------------------------------------------------------------------------------

std::size_t array::resize(const size_t& _size, bool _reallocate)
{
    return this->resize(_size, m_type, _reallocate);
}

//------------------------------------------------------------------------------

void array::clear()
{
    if(!this->m_buffer_object->is_empty())
    {
        if(m_is_buffer_owner)
        {
            this->m_buffer_object->destroy();
        }

        m_strides.clear();
        m_type = core::type::NONE;
        m_size.clear();
    }
}

//------------------------------------------------------------------------------

bool array::empty() const
{
    return m_size.empty();
}

//------------------------------------------------------------------------------

std::size_t array::element_size_in_bytes() const
{
    return m_type.size();
}

//------------------------------------------------------------------------------

std::size_t array::num_elements() const
{
    return compute_size(1, m_size);
}

//------------------------------------------------------------------------------

std::size_t array::size_in_bytes() const
{
    return compute_size(m_type.size(), m_size);
}

//------------------------------------------------------------------------------

const data::array::size_t& array::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------

const data::array::offset_t& array::get_strides() const
{
    return m_strides;
}

//------------------------------------------------------------------------------

std::size_t array::num_dimensions() const
{
    return m_size.size();
}

//------------------------------------------------------------------------------

void array::set_is_buffer_owner(const bool _own)
{
    m_is_buffer_owner = _own;
}

//------------------------------------------------------------------------------

bool array::get_is_buffer_owner() const
{
    return m_is_buffer_owner;
}

//------------------------------------------------------------------------------

core::type array::type() const
{
    return m_type;
}

//------------------------------------------------------------------------------

std::size_t array::get_buffer_offset(const data::array::index_t& _id) const
{
    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Given index has " + std::to_string(_id.size()) + " dimensions, but array has "
            + std::to_string(m_size.size()) + " dimensions."
        ),
        _id.size() != m_size.size()
    );

    offset_t offsets(_id.size());

    std::transform(
        _id.begin(),
        _id.end(),
        m_strides.begin(),
        offsets.begin(),
        std::multiplies<>()
    );

    const std::size_t offset = std::accumulate(offsets.begin(), offsets.end(), static_cast<std::size_t>(0));

    return offset;
}

//------------------------------------------------------------------------------

void* array::buffer()
{
    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "The buffer cannot be accessed if the array is not locked for dump "
            "(see lock())"
        ),
        !m_buffer_object->is_locked()
    );
    return m_buffer_object->buffer();
}

//-----------------------------------------------------------------------------

const void* array::buffer() const
{
    SIGHT_THROW_EXCEPTION_IF(
        data::exception("The buffer cannot be accessed if the array is not locked"),
        !m_buffer_object->is_locked()
    );
    return m_buffer_object->buffer();
}

//------------------------------------------------------------------------------

void array::set_buffer(void* _buf, bool _take_ownership, core::memory::buffer_allocation_policy::sptr _policy)
{
    if(m_buffer_object)
    {
        if(!m_buffer_object->is_empty())
        {
            m_buffer_object->destroy();
        }
    }
    else
    {
        core::memory::buffer_object::sptr new_buffer_object = std::make_shared<core::memory::buffer_object>();
        m_buffer_object->swap(new_buffer_object);
    }

    m_buffer_object->set_buffer(_buf, (_buf == nullptr) ? 0 : this->size_in_bytes(), _policy);
    this->set_is_buffer_owner(_take_ownership);
}

//------------------------------------------------------------------------------

void array::set_buffer(
    void* _buf,
    bool _take_ownership,
    const data::array::size_t& _size,
    const core::type& _type,
    core::memory::buffer_allocation_policy::sptr _policy
)
{
    this->resize(_size, _type, false);
    this->set_buffer(_buf, _take_ownership, _policy);
}

//-----------------------------------------------------------------------------

char* array::get_buffer_ptr(const data::array::index_t& _id)
{
    const std::size_t offset = this->get_buffer_offset(_id);
    char* item               = static_cast<char*>(this->buffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

const char* array::get_buffer_ptr(const data::array::index_t& _id) const
{
    const std::size_t offset = this->get_buffer_offset(_id);
    const char* item         = static_cast<const char*>(this->buffer()) + offset;
    return item;
}

//------------------------------------------------------------------------------

void array::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const
{
    _locks.push_back(m_buffer_object->lock());
}

//------------------------------------------------------------------------------

array::iterator<char> array::begin()
{
    return {static_cast<char*>(buffer())};
}

//------------------------------------------------------------------------------

array::iterator<char> array::end()
{
    auto itr = iterator<char>(static_cast<char*>(buffer()));
    itr += static_cast<std::ptrdiff_t>(this->size_in_bytes());
    return itr;
}

//------------------------------------------------------------------------------

array::const_iterator<char> array::begin() const
{
    return {static_cast<const char*>(buffer())};
}

//------------------------------------------------------------------------------

array::const_iterator<char> array::end() const
{
    auto itr = const_iterator<char>(static_cast<const char*>(buffer()));
    itr += static_cast<std::ptrdiff_t>(this->size_in_bytes());
    return itr;
}

//------------------------------------------------------------------------------

bool array::operator==(const array& _other) const noexcept
{
    if(m_strides != _other.m_strides
       || m_type != _other.m_type
       || m_size != _other.m_size
       || !core::tools::is_equal(m_buffer_object, _other.m_buffer_object))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool array::operator!=(const array& _other) const noexcept
{
    return !(*this == _other);
}

} //namespace sight::data
