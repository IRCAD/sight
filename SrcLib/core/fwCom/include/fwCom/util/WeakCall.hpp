/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_WEAKCALL_HPP__
#define __FWCOM_UTIL_WEAKCALL_HPP__

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <fwCore/mt/types.hpp>

#ifdef COM_LOG
#include "fwCom/SlotBase.hpp"
#endif // COM_LOG
#include "fwCom/util/log.hpp"

namespace fwCom
{

namespace util
{

/**
 * @brief Weak functor class.
 * Store a zero-arg-function and a weak pointer on an object. The WeakCall is
 * callable only once.
 * A bad_weak_ptr exception is thrown if the weak_ptr is expired when operator()
 * is called
 * @tparam T Stored object type.
 * @tparam R Stored function return type.
 */
template< typename T, typename R >
struct WeakCall
{

    WeakCall( const ::boost::shared_ptr< T const > &ptr, ::boost::function< R() > f)
        : m_weakPtr(ptr), m_func(f)
    {
        OSLM_COM("WeakCall object : " << ptr->getFullClassname() );
#ifdef COM_LOG
        this->m_objectType = ptr->getFullClassname();
        ::fwCom::SlotBase::csptr slot = ::boost::dynamic_pointer_cast< const ::fwCom::SlotBase >(ptr);
        if(slot)
        {
            this->m_objectId = slot->getID();
        }
#endif // COM_LOG
    }

    WeakCall( const ::boost::shared_ptr< T const > &ptr, ::boost::function< R() > f, ::fwCore::mt::ReadWriteMutex& m)
        : m_weakPtr(ptr), m_func(f), m_lock( ::boost::make_shared< ::fwCore::mt::ReadLock > (::boost::ref(m)) )
    {
        OSLM_COM("WeakCall object : " << ptr->getFullClassname() );
#ifdef COM_LOG
        this->m_objectType = ptr->getFullClassname();
        ::fwCom::SlotBase::csptr slot = ::boost::dynamic_pointer_cast< const ::fwCom::SlotBase >(ptr);
        if(slot)
        {
            this->m_objectId = slot->getID();
        }
#endif // COM_LOG
    }

    ~WeakCall()
    {

    }

    R operator ()() const
    {
        OSLM_COM("WeakCall : trying to execute WeakCall related to :"
                 << this->m_objectType << ", " << this->m_objectId);

        OSLM_COM_IF( "Failed to execute WeakCall related to :"
                 << this->m_objectType << ", " << this->m_objectId,
                 this->m_weakPtr.expired() );

        ::boost::shared_ptr< T const > ptr(this->m_weakPtr.lock());

        if (!ptr)
        {
            try
            {
                this->m_lock.reset();
            }
            catch(...)
            {
                // may throw an exception if pointed lock locks a destroyed mutex
            }
            // will throw an exception because m_weakPtr is expired
            ::boost::shared_ptr< T const > ptr(this->m_weakPtr);
        }

        ::boost::shared_ptr< ::fwCore::mt::ReadLock > lock(this->m_lock);

        this->m_lock.reset();

        this->m_weakPtr.reset();

        return this->m_func();
    }

protected:
    mutable ::boost::weak_ptr< T const > m_weakPtr;
    ::boost::function< R() > m_func;
    mutable ::boost::shared_ptr< ::fwCore::mt::ReadLock > m_lock;

#ifdef COM_LOG
    std::string m_objectType;
    std::string m_objectId;
#endif // COM_LOG
};


/// Returns weak call from given object and function.
template <typename T, typename R >
WeakCall<T, R> weakcall( const ::boost::shared_ptr< T const > &ptr, ::boost::function< R() > f)
{
    return WeakCall<T, R>(ptr, f);
}

/// Returns weak call from given object, function and mutex.
template <typename T, typename R >
WeakCall<T, R> weakcall( const ::boost::shared_ptr< T const > &ptr, ::boost::function< R() > f, ::fwCore::mt::ReadWriteMutex& m)
{
    return WeakCall<T, R>(ptr, f, m);
}



} // namespace util

} // namespace fwCom

#endif /* __FWCOM_UTIL_WEAKCALL_HPP__ */

