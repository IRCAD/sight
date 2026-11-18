/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#pragma once

#include <sight/core/config.hpp>

#include "core/memory/exception/memory.hpp"

#include <core/base.hpp>
#include <core/exceptionmacros.hpp>

#include <algorithm>
#include <new>

namespace sight::core::memory
{

class SIGHT_CORE_CLASS_API buffer_allocation_policy
{
public:

    using sptr      = std::shared_ptr<buffer_allocation_policy>;
    using size_type = std::size_t;
    using buffer_t  = void*;

    SIGHT_CORE_API virtual void allocate(buffer_t& _buffer, size_type _size)   = 0;
    SIGHT_CORE_API virtual void reallocate(buffer_t& _buffer, size_type _size) = 0;
    SIGHT_CORE_API virtual void destroy(buffer_t& _buffer)                     = 0;

    //------------------------------------------------------------------------------

    virtual void reallocate(buffer_t& _buffer, size_type _size, size_type /*_old_size*/)
    {
        reallocate(_buffer, _size);
    }

    SIGHT_CORE_API virtual ~buffer_allocation_policy()
    = default;
};

class SIGHT_CORE_CLASS_API buffer_malloc_policy : public buffer_allocation_policy
{
public:

    SIGHT_CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void destroy(buffer_t& _buffer) override;
};

/**
 * @brief Allocates a buffer with new[], as an array of ELEMENT, and frees it with delete[] of the same type.
 *
 * A buffer that comes from a new ELEMENT[] elsewhere, as the pixels of an itk::Image, must be freed with this ELEMENT.
 * reallocate() keeps the content, as realloc() does. The buffer manager supplies the previous size; direct calls to
 * the two-argument overload require a buffer allocated by this policy or an initial size passed to its constructor.
 *
 * @code{.cpp}
 * bo->allocate(size, std::make_shared<core::memory::buffer_new_policy<> >());
 * image.set_buffer(pixels, true, type, size, format, std::make_shared<core::memory::buffer_new_policy<float> >(bytes));
 * @endcode
 */
template<typename ELEMENT = char>
class buffer_new_policy final : public buffer_allocation_policy
{
public:

    buffer_new_policy() noexcept = default;

    /// @param _size the size, in bytes, of a buffer allocated by new ELEMENT[] that this policy takes over.
    explicit buffer_new_policy(size_type _size) noexcept :
        m_size(_size)
    {
    }

    //------------------------------------------------------------------------------

    void allocate(buffer_t& _buffer, size_type _size) override
    {
        _buffer  = new_elements(_size);
        m_buffer = _buffer;
        m_size   = _size;
    }

    //------------------------------------------------------------------------------

    void reallocate(buffer_t& _buffer, size_type _size) override
    {
        if(_buffer != nullptr && (m_size == 0 || (m_buffer != nullptr && m_buffer != _buffer)))
        {
            SIGHT_THROW_EXCEPTION_MSG(
                core::memory::exception::memory,
                "Cannot reallocate a buffer whose size is not known to this allocation policy."
            );
        }

        reallocate(_buffer, _size, m_size);
    }

    //------------------------------------------------------------------------------

    void reallocate(buffer_t& _buffer, size_type _size, size_type _old_size) override
    {
        buffer_t resized = new_elements(_size);
        if(resized != nullptr && _buffer != nullptr)
        {
            std::copy_n(static_cast<const char*>(_buffer), std::min(_old_size, _size), static_cast<char*>(resized));
        }

        destroy(_buffer);
        _buffer  = resized;
        m_buffer = resized;
        m_size   = _size;
    }

    //------------------------------------------------------------------------------

    void destroy(buffer_t& _buffer) override
    {
        if(m_buffer == _buffer || m_buffer == nullptr)
        {
            m_buffer = nullptr;
            m_size   = 0;
        }

        delete[] static_cast<ELEMENT*>(_buffer);
        _buffer = nullptr;
    }

private:

    //------------------------------------------------------------------------------

    static ELEMENT* new_elements(size_type _size)
    {
        if(_size == 0)
        {
            return nullptr;
        }

        try
        {
            // Rounded up without overflow, so that the largest sizes are refused rather than wrapped.
            return new ELEMENT[_size / sizeof(ELEMENT) + (_size % sizeof(ELEMENT) != 0 ? 1 : 0)];
        }
        catch(const std::bad_alloc& exception)
        {
            SIGHT_THROW_EXCEPTION_MSG(core::memory::exception::memory, "bad_alloc caught: " << exception.what());
        }
    }

    buffer_t m_buffer {nullptr};
    size_type m_size {0};
};

class SIGHT_CORE_CLASS_API buffer_no_alloc_policy : public buffer_allocation_policy
{
public:

    SIGHT_CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void destroy(buffer_t& _buffer) override;
};

} // namespace sight::core::memory
