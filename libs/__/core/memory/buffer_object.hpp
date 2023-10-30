/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"
#include "core/memory/buffer_allocation_policy.hpp"
#include "core/memory/buffer_manager.hpp"

#include <filesystem>
#include <istream>
#include <type_traits>

namespace sight::core::memory
{

namespace stream::in
{

class factory;

} // namespace stream::in

/**
 * @brief   Define Base class for Sight buffers
 *
 * Keep a pointer to a buffer and it's allocation policy (new malloc) without
 * any cleverness about allocating/destroying the buffer. Users of this class
 * needs to take care about allocation and destruction by themselves.
 *
 * BufferObject class has a BufferManager and Locks mechanism, Allowing to
 * trigger special treatments on various events on BufferObjects (allocation,
 * reallocation, destruction, swapping, locking, unlocking) and allowing to
 * give some guarantees.
 *
 * Users of buffer have to keep a lock on a BufferObject when dealing with the
 * buffers content. Keeping a lock on a BufferObject guarantees that the buffer
 * will not be changed or modified by the BufferManager mechanism. A lock *DO
 * NOT ENSURE* that an other user of this buffer object are not
 * changing/modifying the buffer.
 */
class CORE_CLASS_API buffer_object : public sight::core::base_object
{
public:

    using counter_type      = std::shared_ptr<void>;
    using weak_counter_type = std::weak_ptr<void>;

    using size_t = std::size_t;

    SIGHT_DECLARE_CLASS(buffer_object, core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// return the sub class classname : an alias of this->get_classname
    std::string class_name() const
    {
        return this->get_classname();
    }

    //------------------------------------------------------------------------------

    virtual core::memory::buffer_manager::buffer_t buffer() const
    {
        return m_buffer;
    }

    /**
     * @brief base class for BufferObject Lock
     *
     * This class purpose is to provide a way to count buffer uses, to prevent
     * BufferManager changes on buffer if nb uses > 0
     *
     * The count is shared with the associated BufferObject. Be aware that this
     * mechanism is actually not thread-safe.
     *
     */
    template<typename T>
    class lock_base
    {
    public:

        using buffer_t = typename std::conditional_t<std::is_const_v<T>, const void*, void*>;

        lock_base() = default;
        inline ~lock_base()
        {
            // Resetting the counter in the destructor **BEFORE** resetting BufferObject shared pointer is required !
            // Otherwise, the lock count assert in the destruction of the buffer, in
            // BufferManager::::unregisterBufferImpl() will be triggered.
            m_count.reset();
        }

        /**
         * @brief Build a lock on object 'bo'
         *
         * Increments BufferObject's lock counts.
         *
         * @param bo BufferObject to lock
         */
        lock_base(const SPTR(T)& _bo) :
            m_count(_bo->m_count.lock()),
            m_buffer_object(_bo)
        {
            SIGHT_ASSERT("Can't lock NULL object", _bo);

            core::mt::scoped_lock lock(_bo->m_lock_dump_mutex);

            if(!m_count)
            {
                m_count      = _bo->m_buffer_manager->lock_buffer(&(_bo->m_buffer)).get();
                _bo->m_count = m_count;
            }
        }

        /**
         * @brief Returns BufferObject's buffer pointer
         */
        [[nodiscard]] typename lock_base<T>::buffer_t buffer() const
        {
            return m_buffer_object->m_buffer;
        }

        /**
         * @brief Release any count on any Buffer the lock may have.
         */
        void reset()
        {
            m_count.reset();
            m_buffer_object.reset();
        }

    protected:

        buffer_object::counter_type m_count;
        // Using a shared_ptr allows to keep the buffer alive until the lock is destroyed,
        // otherwise we would raise the lock count assert in the destruction of the buffer,
        // in BufferManager::::unregisterBufferImpl()
        SPTR(T) m_buffer_object;
    };

    /**
     * @name Locks
     * @brief Locks types
     * @{
     */
    using lock_t       = lock_base<buffer_object>;
    using const_lock_t = lock_base<const buffer_object>;
    /**  @} */

    /**
     * @brief BufferObject constructor
     *
     * Register the buffer to an existing buffer manager.
     */
    CORE_API buffer_object(bool _auto_delete = false);

    /**
     * @brief BufferObject destructor
     *
     * unregister the buffer from the buffer manager.
     */
    CORE_API ~buffer_object() override;

    /**
     * @brief Buffer allocation
     *
     * Allocate a buffer using given policy.
     * The allocation may have been hooked by the buffer manager.
     *
     * @param size number of bytes to allocate
     * @param policy Buffer allocation policy, default is Malloc policy
     *
     */
    CORE_API virtual void allocate(
        size_t _size,
        const core::memory::buffer_allocation_policy::sptr& _policy =
        std::make_shared<core::memory::buffer_malloc_policy>()
    );

    /**
     * @brief Buffer reallocation
     *
     * Reallocate the buffer using the associated policy. A policy may not
     * handle reallocation.
     * The reallocation may have been hooked by the buffer manager.
     *
     * @param size New buffer size
     *
     */
    CORE_API virtual void reallocate(size_t _size);

    /**
     * @brief Buffer deallocation
     *
     * Destroy the buffer using the associated policy.
     * The destruction may have been hooked by the buffer manager.
     *
     */
    CORE_API virtual void destroy();

    /**
     * @brief Buffer setter
     *
     * Set the buffer from an existing one.
     *
     * @param buffer External Buffer
     * @param size Buffer's size
     * @param policy External buffer allocation policy, default is Malloc policy
     *
     */
    CORE_API virtual void set_buffer(
        core::memory::buffer_manager::buffer_t _buffer,
        size_t _size,
        const core::memory::buffer_allocation_policy::sptr& _policy =
        std::make_shared<core::memory::buffer_malloc_policy>(),
        bool _auto_delete = false
    );

    /**
     * @brief Return a lock on the BufferObject
     *
     * @return Lock on the BufferObject
     */
    CORE_API virtual lock_t lock();

    /**
     * @brief Return a const lock on the BufferObject
     *
     * @return ConstLock on the BufferObject
     */
    CORE_API virtual const_lock_t lock() const;

    /**
     * @brief Returns the buffer's size
     */
    size_t size() const
    {
        return m_size;
    }

    /**
     * @brief Returns true if the buffer is empty
     */
    bool is_empty() const
    {
        return m_size == 0;
    }

    /**
     * @brief Returns the number of locks on the BufferObject
     */
    std::int64_t lock_count() const
    {
        return m_count.use_count();
    }

    /**
     * @brief Returns true if the buffer has any lock
     */
    bool is_locked() const
    {
        return lock_count() != 0;
    }

    /**
     * @brief Returns pointer on BufferObject's buffer
     */
    core::memory::buffer_manager::const_buffer_ptr_t get_buffer_pointer() const
    {
        return &m_buffer;
    }

    //------------------------------------------------------------------------------

    core::mt::read_write_mutex& get_mutex()
    {
        return m_mutex;
    }

    /// Exchanges the content of the BufferObject with the content of _source.
    CORE_API void swap(const buffer_object::sptr& _source) noexcept;

    CORE_API buffer_manager::stream_info get_stream_info() const;

    /**
     * @brief Set a stream factory for the buffer manager
     * The factory will be used to load data on demand by the buffer manager.
     *
     * @param factory core::memory::stream::in::IFactory stream factory
     * @param size size of data provided by the stream
     * @param sourceFile Filesystem path of the source file, if applicable
     * @param format file format (RAW,RAWZ,OTHER), if sourceFile is provided
     * @param policy Buffer allocation policy
     */
    CORE_API void set_istream_factory(
        const SPTR(core::memory::stream::in::factory)& _factory,
        size_t _size,
        const std::filesystem::path& _source_file                   = "",
        core::memory::file_format_type _format                      = core::memory::other,
        const core::memory::buffer_allocation_policy::sptr& _policy = std::make_shared<core::memory::buffer_malloc_policy>()
    );

    /// Equality comparison operators
    /// @{
    CORE_API bool operator==(const buffer_object& _other) const noexcept;
    CORE_API bool operator!=(const buffer_object& _other) const noexcept;

    /// @}

protected:

    core::memory::buffer_manager::buffer_t m_buffer {nullptr};

    size_t m_size {0};

    mutable weak_counter_type m_count;
    mutable core::mt::mutex m_lock_dump_mutex;
    core::mt::read_write_mutex m_mutex;

    core::memory::buffer_manager::sptr m_buffer_manager;

    core::memory::buffer_allocation_policy::sptr m_alloc_policy;

    bool m_auto_delete {false};
};

} // namespace sight::core::memory
