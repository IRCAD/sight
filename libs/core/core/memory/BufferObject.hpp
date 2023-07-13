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
#include "core/memory/BufferAllocationPolicy.hpp"
#include "core/memory/BufferManager.hpp"

#include <filesystem>
#include <istream>
#include <type_traits>

namespace sight::core::memory
{

namespace stream::in
{

class IFactory;

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
class CORE_CLASS_API BufferObject : public sight::core::BaseObject
{
public:

    typedef std::shared_ptr<void> CounterType;
    typedef std::weak_ptr<void> WeakCounterType;

    typedef std::size_t SizeType;

    SIGHT_DECLARE_CLASS(BufferObject, core::BaseObject, new BufferObject);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Returns a auto-deleted BufferObject
    static sptr New(bool autoDelete)
    {
        return std::make_shared<BufferObject>(autoDelete);
    }

    /// return the sub class classname : an alias of this->getClassname
    std::string className() const
    {
        return this->getClassname();
    }

    //------------------------------------------------------------------------------

    virtual core::memory::BufferManager::BufferType getBuffer() const
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
    class LockBase
    {
    public:

        typedef typename std::conditional_t<std::is_const_v<T>, const void*, void*> BufferType;

        LockBase() = default;
        inline ~LockBase()
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
        LockBase(const SPTR(T)& bo) :
            m_count(bo->m_count.lock()),
            m_bufferObject(bo)
        {
            SIGHT_ASSERT("Can't lock NULL object", bo);

            core::mt::ScopedLock lock(bo->m_lockDumpMutex);

            if(!m_count)
            {
                m_count     = bo->m_bufferManager->lockBuffer(&(bo->m_buffer)).get();
                bo->m_count = m_count;
            }
        }

        /**
         * @brief Returns BufferObject's buffer pointer
         */
        [[nodiscard]] typename LockBase<T>::BufferType getBuffer() const
        {
            return m_bufferObject->m_buffer;
        }

        /**
         * @brief Release any count on any Buffer the lock may have.
         */
        void reset()
        {
            m_count.reset();
            m_bufferObject.reset();
        }

    protected:

        BufferObject::CounterType m_count;
        // Using a shared_ptr allows to keep the buffer alive until the lock is destroyed,
        // otherwise we would raise the lock count assert in the destruction of the buffer,
        // in BufferManager::::unregisterBufferImpl()
        SPTR(T) m_bufferObject;
    };

    /**
     * @name Locks
     * @brief Locks types
     * @{
     */
    typedef LockBase<BufferObject> Lock;
    typedef LockBase<const BufferObject> ConstLock;
    /**  @} */

    /**
     * @brief BufferObject constructor
     *
     * Register the buffer to an existing buffer manager.
     */
    CORE_API BufferObject(bool autoDelete = false);

    /**
     * @brief BufferObject destructor
     *
     * unregister the buffer from the buffer manager.
     */
    CORE_API ~BufferObject() override;

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
        SizeType size,
        const core::memory::BufferAllocationPolicy::sptr& policy =
        core::memory::BufferMallocPolicy::New()
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
    CORE_API virtual void reallocate(SizeType size);

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
    CORE_API virtual void setBuffer(
        core::memory::BufferManager::BufferType buffer,
        SizeType size,
        const core::memory::BufferAllocationPolicy::sptr& policy =
        core::memory::BufferMallocPolicy::New(),
        bool autoDelete = false
    );

    /**
     * @brief Return a lock on the BufferObject
     *
     * @return Lock on the BufferObject
     */
    CORE_API virtual Lock lock();

    /**
     * @brief Return a const lock on the BufferObject
     *
     * @return ConstLock on the BufferObject
     */
    CORE_API virtual ConstLock lock() const;

    /**
     * @brief Returns the buffer's size
     */
    SizeType getSize() const
    {
        return m_size;
    }

    /**
     * @brief Returns true if the buffer is empty
     */
    bool isEmpty() const
    {
        return m_size == 0;
    }

    /**
     * @brief Returns the number of locks on the BufferObject
     */
    std::int64_t lockCount() const
    {
        return m_count.use_count();
    }

    /**
     * @brief Returns true if the buffer has any lock
     */
    bool isLocked() const
    {
        return lockCount() != 0;
    }

    /**
     * @brief Returns pointer on BufferObject's buffer
     */
    core::memory::BufferManager::ConstBufferPtrType getBufferPointer() const
    {
        return &m_buffer;
    }

    //------------------------------------------------------------------------------

    core::mt::ReadWriteMutex& getMutex()
    {
        return m_mutex;
    }

    /// Exchanges the content of the BufferObject with the content of _source.
    CORE_API void swap(const BufferObject::sptr& _source);

    CORE_API BufferManager::StreamInfo getStreamInfo() const;

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
    CORE_API void setIStreamFactory(
        const SPTR(core::memory::stream::in::IFactory)& factory,
        SizeType size,
        const std::filesystem::path& sourceFile                  = "",
        core::memory::FileFormatType format                      = core::memory::OTHER,
        const core::memory::BufferAllocationPolicy::sptr& policy = core::memory::BufferMallocPolicy::New()
    );

    /// Equality comparison operators
    /// @{
    CORE_API bool operator==(const BufferObject& other) const noexcept;
    CORE_API bool operator!=(const BufferObject& other) const noexcept;

    /// @}

protected:

    core::memory::BufferManager::BufferType m_buffer {nullptr};

    SizeType m_size {0};

    mutable WeakCounterType m_count;
    mutable core::mt::Mutex m_lockDumpMutex;
    core::mt::ReadWriteMutex m_mutex;

    core::memory::BufferManager::sptr m_bufferManager;

    core::memory::BufferAllocationPolicy::sptr m_allocPolicy;

    bool m_autoDelete {false};
};

} // namespace sight::core::memory
