/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/memory/buffer_object.hpp"

namespace sight::core::memory
{

buffer_object::buffer_object(bool _auto_delete) :
    m_buffer_manager(core::memory::buffer_manager::get()),
    m_alloc_policy(std::make_shared<core::memory::buffer_no_alloc_policy>()),
    m_auto_delete(_auto_delete)
{
    m_buffer_manager->register_buffer(&m_buffer).get();
}

//------------------------------------------------------------------------------

buffer_object::~buffer_object()
{
    // If the buffer is not owned like in sight::data::array, we must destroy it by ourselves
    ///@todo remove this when sight::data::dicom_series will be removed
    if(m_auto_delete)
    {
        m_buffer_manager->destroy_buffer(&m_buffer).get();
    }

    // In the past we asserted that m_count was expired, but it can not be ensured because the unlock is asynchronous
    // So we simply unregister the buffer and we will check the counter value on the buffer manager thread instead
    m_buffer_manager->unregister_buffer(&m_buffer).get();
}

//------------------------------------------------------------------------------

void buffer_object::allocate(size_t _size, const core::memory::buffer_allocation_policy::sptr& _policy)
{
    m_buffer_manager->allocate_buffer(&m_buffer, _size, _policy).get();
    m_alloc_policy = _policy;
    m_size         = _size;
}

//------------------------------------------------------------------------------

void buffer_object::reallocate(size_t _size)
{
    m_buffer_manager->reallocate_buffer(&m_buffer, _size).get();
    m_size = _size;
}

//------------------------------------------------------------------------------

void buffer_object::destroy()
{
    m_buffer_manager->destroy_buffer(&m_buffer).get();
    m_alloc_policy = std::make_shared<core::memory::buffer_no_alloc_policy>();
    m_size         = 0;
}

//------------------------------------------------------------------------------

void buffer_object::set_buffer(
    core::memory::buffer_manager::buffer_t _buffer,
    size_t _size,
    const core::memory::buffer_allocation_policy::sptr& _policy,
    bool _auto_delete
)
{
    m_buffer_manager->set_buffer(&m_buffer, _buffer, _size, _policy).get();
    m_alloc_policy = _policy;
    m_size         = _size;
    m_auto_delete  = _auto_delete;
}

//------------------------------------------------------------------------------

buffer_object::lock_t buffer_object::lock()
{
    return {this->get_sptr()};
}

//------------------------------------------------------------------------------

buffer_object::const_lock_t buffer_object::lock() const
{
    return {this->get_const_sptr()};
}

//------------------------------------------------------------------------------

void buffer_object::swap(const buffer_object::sptr& _source) noexcept
{
    m_buffer_manager->swap_buffer(&m_buffer, &(_source->m_buffer)).get();

    std::swap(m_size, _source->m_size);
    m_buffer_manager.swap(_source->m_buffer_manager);
    m_alloc_policy.swap(_source->m_alloc_policy);
}

//------------------------------------------------------------------------------

buffer_manager::stream_info buffer_object::get_stream_info() const
{
    return m_buffer_manager->get_stream_info(&m_buffer).get();
}

//------------------------------------------------------------------------------

void buffer_object::set_istream_factory(
    const SPTR(core::memory::stream::in::factory)& _factory,
    size_t _size,
    const std::filesystem::path& _source_file,
    core::memory::file_format_type _format,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    m_size         = _size;
    m_alloc_policy = _policy;
    m_buffer_manager->set_istream_factory(&m_buffer, _factory, _size, _source_file, _format, _policy).get();
}

//------------------------------------------------------------------------------

bool buffer_object::operator==(const buffer_object& _other) const noexcept
{
    return m_buffer == _other.m_buffer
           || (m_size == _other.m_size && std::memcmp(m_buffer, _other.m_buffer, m_size) == 0);
}

//------------------------------------------------------------------------------

bool buffer_object::operator!=(const buffer_object& _other) const noexcept
{
    return !(*this == _other);
}

} //namespace sight::core::memory
