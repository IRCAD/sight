/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_BUFFEROBJECT_HPP_
#define _FWTOOLS_BUFFEROBJECT_HPP_

#include <boost/type_traits/conditional.hpp>
#include <boost/type_traits/is_const.hpp>

#include <fwCore/base.hpp>

#include "fwTools/BufferAllocationPolicy.hpp"
#include "fwTools/IBufferManager.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{
/**
 * @brief   Define Base class for FW4SPL buffers
 * @class   BufferObject
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
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
class FWTOOLS_CLASS_API BufferObject : public ::fwCore::BaseObject
{
public:

    typedef size_t SizeType;

    fwCoreClassDefinitionsWithFactoryMacro((BufferObject), (()), new BufferObject );
    fwCoreAllowSharedFromThis();

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

        typedef typename ::boost::conditional< ::boost::is_const< T >::value, const void*, void* >::type BufferType;


        /**
         * @brief Build an empty lock.
         */
        LockBase() :
            m_count(0)
        {
        }

        /**
         * @brief Build a lock on object 'bo'
         *
         * Increments BufferObject's lock counts.
         *
         * @param bo BufferObject to lock
         */
        LockBase( SPTR(T) bo ) :
            m_count(0),
            m_bufferObject(bo)
        {
            SLM_ASSERT("Can't lock NULL object", bo);
            m_count = bo->m_count;
            SLM_ASSERT("Count pointer is NULL", m_count != 0);
            ++(*m_count);
            this->lock();
        }

        /**
         * @brief Copy constructor
         *
         * If the copied lock has a lock count, increment it.
         *
         * @param other Lock to copy
         */
        LockBase( const LockBase &other ):
            m_count(0)
        {
            m_count = other.m_count;
            if (m_count)
            {
                SLM_ASSERT("Can't lock NULL object", other.m_bufferObject.lock());
                ++(*m_count);
                m_bufferObject = other.m_bufferObject;
                this->lock();
            }
        }

        /**
         * @brief Lock destructor
         *
         * Decrement a lock count if any.
         */
        ~LockBase()
        {
            if (m_count)
            {
                --(*m_count);
                this->unlock();
            }
        }

        /**
         * @brief copy operator
         *
         * Decrement count if has any, and if the copied lock has a lock count,
         * increment it.
         *
         * @param other lock to copy
         *
         * @return the lock itself
         */
        LockBase & operator= (const LockBase & other)
        {
            if (m_count)
            {
                --(*m_count);
                this->unlock();
            }
            m_count = other.m_count;
            if (m_count)
            {
                ++(*m_count);
                m_bufferObject = other.m_bufferObject;
                this->lock();
            }
            return *this;
        }

        /**
         * @brief Returns BufferObject's buffer pointer
         *
         * @return
         */
        BufferType getBuffer() const
        {
            SPTR(T) bufferObject = m_bufferObject.lock();
            BufferType buffer = bufferObject->m_buffer;
            return buffer;
        };

        /**
         * @brief Release any count on any BufferType the lock may have.
         */
        void reset()
        {
            if (m_count)
            {
                --(*m_count);
                this->unlock();
            }
            m_count = 0;
            m_bufferObject.reset();
        }

    protected:

        /**
         * @brief If any instance of BufferManager is active, triggers t's lockBuffer event.
         */
        void lock()
        {
            SLM_ASSERT("Count pointer is NULL", m_count != 0);
            if( SPTR(T) bufferObject = m_bufferObject.lock() )
            {
                if (fwTools::IBufferManager::sptr manager = bufferObject->m_bufferManager)
                {
                    manager->lockBuffer(&(bufferObject->m_buffer));
                }
            }
        }

        /**
         * @brief If any instance of BufferManager is active, triggers t's unlockBuffer event.
         */
        void unlock()
        {
            SLM_ASSERT("Count pointer is NULL", m_count != 0);
            if( SPTR(T) bufferObject = m_bufferObject.lock() )
            {
                if (fwTools::IBufferManager::sptr manager = bufferObject->m_bufferManager)
                {
                    manager->unlockBuffer(&(bufferObject->m_buffer));
                }
            }
        }



        long *m_count;
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
    FWTOOLS_API BufferObject();

    /**
     * @brief BufferObject destructor
     *
     * unregister the buffer from the buffer manager.
     */
    FWTOOLS_API virtual ~BufferObject();


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
    FWTOOLS_API virtual void allocate(SizeType size,
            ::fwTools::BufferAllocationPolicy::sptr policy = ::fwTools::BufferMallocPolicy::New());

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
    FWTOOLS_API virtual void reallocate(SizeType size);

    /**
     * @brief Buffer deallocation
     *
     * Destroy the buffer using the associated policy.
     * The destruction may have been hooked by the buffer manager.
     *
     */
    FWTOOLS_API virtual void destroy();


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
    FWTOOLS_API virtual void setBuffer(void *buffer, SizeType size,
            ::fwTools::BufferAllocationPolicy::sptr policy = ::fwTools::BufferMallocPolicy::New());


    /**
     * @brief Return a lock on the BufferObject
     *
     * @return Lock on the BufferObject
     */
    FWTOOLS_API virtual Lock lock();

    /**
     * @brief Return a const lock on the BufferObject
     *
     * @return ConstLock on the BufferObject
     */
    FWTOOLS_API virtual ConstLock lock() const;

    /**
     * @brief Returns the buffer's size
     */
    SizeType getSize() const { return m_size; };

    /**
     * @brief Returns true if the buffer is empty
     */
    bool isEmpty() const { return m_size == 0; };

    /**
     * @brief Returns the number of locks on the BufferObject
     */
    long lockCount() const { return *m_count; };

    /**
     * @brief Returns true if the buffer has any lock
     */
    long isLocked() const { return lockCount() != 0; };

    /**
     * @brief Returns pointer on BufferObject's buffer
     */
    const void * const getBufferPointer() const {return &m_buffer;};

    /// Exchanges the content of the BufferObject with the content of _source.
    FWTOOLS_API void swap( BufferObject::sptr _source );

protected :

    void *m_buffer;

    SizeType m_size;

    long *m_count;

    ::fwTools::IBufferManager::sptr m_bufferManager;

    ::fwTools::BufferAllocationPolicy::sptr m_allocPolicy;
};

}

#endif /* _FWTOOLS_BUFFEROBJECT_HPP_ */
