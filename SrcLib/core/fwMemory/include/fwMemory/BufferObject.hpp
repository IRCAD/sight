/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwMemory/BufferAllocationPolicy.hpp"
#include "fwMemory/BufferManager.hpp"
#include "fwMemory/config.hpp"

#include <fwCamp/macros.hpp>

#include <filesystem>

#include <istream>
#include <type_traits>

fwCampAutoDeclareMacro((fwMemory)(BufferObject), FWMEMORY_API);

namespace fwMemory
{

namespace stream
{
namespace in
{
class IFactory;
}
}

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
 * NOT GARANTEE* that an other user of this buffer object are not
 * changing/modifying the buffer.
 */
class FWMEMORY_CLASS_API BufferObject : public ::fwCore::BaseObject
{

public:

    typedef std::shared_ptr< void > CounterType;
    typedef std::weak_ptr< void > WeakCounterType;

    typedef size_t SizeType;

    fwCoreClassMacro(BufferObject, ::fwCore::BaseObject, new BufferObject);
    fwCoreAllowSharedFromThis();

    /// return the sub class classname : an alias of this->getClassname
    std::string className() const
    {
        return this->getClassname();
    }

    //------------------------------------------------------------------------------

    virtual ::fwMemory::BufferManager::BufferType getBuffer() const
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
    template <typename T>
    class LockBase
    {
    public:

        typedef typename ::boost::conditional< std::is_const< T >::value, const void*, void* >::type BufferType;

        /**
         * @brief Build an empty lock.
         */
        LockBase()
        {
        }

        /**
         * @brief Build a lock on object 'bo'
         *
         * Increments BufferObject's lock counts.
         *
         * @param bo BufferObject to lock
         */
        LockBase( const SPTR(T)& bo ) :
            m_bufferObject(bo)
        {
            SLM_ASSERT("Can't lock NULL object", bo);

            ::fwCore::mt::ScopedLock lock(bo->m_lockDumpMutex);
            m_count = bo->m_count.lock();
            if ( !m_count )
            {
                m_count     = bo->m_bufferManager->lockBuffer(&(bo->m_buffer)).get();
                bo->m_count = m_count;
            }

        }

        /**
         * @brief Returns BufferObject's buffer pointer
         */
        typename LockBase<T>::BufferType getBuffer() const
        {
            SPTR(T) bufferObject = m_bufferObject.lock();
            BufferType buffer = bufferObject->m_buffer;
            return buffer;
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
        WPTR(T) m_bufferObject;
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
    FWMEMORY_API BufferObject();

    /**
     * @brief BufferObject destructor
     *
     * unregister the buffer from the buffer manager.
     */
    FWMEMORY_API virtual ~BufferObject();

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
    FWMEMORY_API virtual void allocate(SizeType size,
                                       const ::fwMemory::BufferAllocationPolicy::sptr& policy =
                                           ::fwMemory::BufferMallocPolicy::New());

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
    FWMEMORY_API virtual void reallocate(SizeType size);

    /**
     * @brief Buffer deallocation
     *
     * Destroy the buffer using the associated policy.
     * The destruction may have been hooked by the buffer manager.
     *
     */
    FWMEMORY_API virtual void destroy();

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
    FWMEMORY_API virtual void setBuffer(::fwMemory::BufferManager::BufferType buffer, SizeType size,
                                        const ::fwMemory::BufferAllocationPolicy::sptr& policy =
                                            ::fwMemory::BufferMallocPolicy::New());

    /**
     * @brief Return a lock on the BufferObject
     *
     * @return Lock on the BufferObject
     */
    FWMEMORY_API virtual Lock lock();

    /**
     * @brief Return a const lock on the BufferObject
     *
     * @return ConstLock on the BufferObject
     */
    FWMEMORY_API virtual ConstLock lock() const;

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
    long lockCount() const
    {
        return m_count.use_count();
    }

    /**
     * @brief Returns true if the buffer has any lock
     */
    long isLocked() const
    {
        return lockCount() != 0;
    }

    /**
     * @brief Returns pointer on BufferObject's buffer
     */
    const ::fwMemory::BufferManager::ConstBufferPtrType getBufferPointer() const
    {
        return &m_buffer;
    }

    //------------------------------------------------------------------------------

    ::fwCore::mt::ReadWriteMutex& getMutex()
    {
        return m_mutex;
    }

    /// Exchanges the content of the BufferObject with the content of _source.
    FWMEMORY_API void swap( const BufferObject::sptr& _source );

    FWMEMORY_API BufferManager::StreamInfo getStreamInfo() const;

    /**
     * @brief Set a stream factory for the buffer manager
     * The factory will be used to load data on demand by the buffer manager.
     *
     * @param factory ::fwMemory::stream::in::IFactory stream factory
     * @param size size of data provided by the stream
     * @param sourceFile Filesystem path of the source file, if applicable
     * @param format file format (RAW,RAWZ,OTHER), if sourceFile is provided
     * @param policy Buffer allocation policy
     */
    FWMEMORY_API void setIStreamFactory(const SPTR(::fwMemory::stream::in::IFactory)& factory,
                                        SizeType size,
                                        const std::filesystem::path& sourceFile                = "",
                                        ::fwMemory::FileFormatType format                      = ::fwMemory::OTHER,
                                        const ::fwMemory::BufferAllocationPolicy::sptr& policy = ::fwMemory::BufferMallocPolicy::New()
                                        );

protected:

    ::fwMemory::BufferManager::BufferType m_buffer;

    SizeType m_size;

    mutable WeakCounterType m_count;
    mutable ::fwCore::mt::Mutex m_lockDumpMutex;
    ::fwCore::mt::ReadWriteMutex m_mutex;

    ::fwMemory::BufferManager::sptr m_bufferManager;

    ::fwMemory::BufferAllocationPolicy::sptr m_allocPolicy;
};

}
