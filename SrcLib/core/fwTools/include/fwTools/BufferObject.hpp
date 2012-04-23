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
 */
class FWTOOLS_CLASS_API BufferObject : public ::fwCore::BaseObject
{
public:

    typedef size_t SizeType;

    fwCoreClassDefinitionsWithFactoryMacro((BufferObject), (()), new BufferObject );
    fwCoreAllowSharedFromThis();

    template <typename T>
    class LockBase
    {
    public:

        typedef typename ::boost::conditional< ::boost::is_const< T >::value, const void*, void* >::type BufferType;


        LockBase() :
            m_count(0)
        {
        }

        LockBase( SPTR(T) bo ) :
            m_bufferObject(bo),
            m_count(0)
        {
            SLM_ASSERT("Can't lock NULL object", bo);
            m_count = bo->m_count;
            SLM_ASSERT("Count pointer is NULL", m_count != 0);
            ++(*m_count);
            this->lock();
        }

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

        ~LockBase()
        {
            if (m_count)
            {
                --(*m_count);
                this->unlock();
            }
        }

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

        BufferType getBuffer() const
        {
            SPTR(T) bufferObject = m_bufferObject.lock();
            BufferType buffer = bufferObject->m_buffer;
            return buffer;
        };

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


    typedef LockBase<BufferObject> Lock;
    typedef LockBase<const BufferObject> ConstLock;


    FWTOOLS_API BufferObject();

    FWTOOLS_API virtual ~BufferObject();


    FWTOOLS_API virtual void allocate(SizeType size,
            ::fwTools::BufferAllocationPolicy::sptr policy = ::fwTools::BufferMallocPolicy::New());

    FWTOOLS_API virtual void reallocate(SizeType size);
    FWTOOLS_API virtual void destroy();


    FWTOOLS_API virtual void setBuffer(void *buffer, SizeType size,
            ::fwTools::BufferAllocationPolicy::sptr policy = ::fwTools::BufferMallocPolicy::New());


    FWTOOLS_API virtual Lock lock();
    FWTOOLS_API virtual ConstLock lock() const;

    FWTOOLS_API SizeType getSize() const { return m_size; };
    FWTOOLS_API bool isEmpty() const { return m_size == 0; };

    FWTOOLS_API long lockCount() const { return *m_count; };
    FWTOOLS_API long isLocked() const { return lockCount() != 0; };

protected :


    void *m_buffer;

    SizeType m_size;

    long *m_count;

    ::fwTools::IBufferManager::sptr m_bufferManager;

    ::fwTools::BufferAllocationPolicy::sptr m_allocPolicy;
};

}

#endif /* _FWTOOLS_BUFFEROBJECT_HPP_ */
