/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_UTIL_WEAKCALL_HPP__
#define __FWCOM_UTIL_WEAKCALL_HPP__

#include "fwCom/exception/WorkerChanged.hpp"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/mt/types.hpp>

#include <functional>

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
 * A ::fwCom::exception::WorkerChanged exception is thrown if the target slot
 * worker changed since weakcall request
 * @tparam T Stored object type.
 * @tparam R Stored function return type.
 */
template< typename T, typename R >
struct WeakCall
{

    WeakCall( const std::shared_ptr< T const >& ptr, std::function< R() > f) :
        m_weakPtr(ptr),
        m_func(f)
    {
        SLM_TRACE("WeakCall object : " + ptr->getFullClassname() );
    }

    WeakCall( const std::shared_ptr< T const >& ptr, std::function< R() > f,
              const std::shared_ptr< ::fwThread::Worker >& m) :
        m_weakPtr(ptr),
        m_func(f),
        m_worker( m )
    {
        SLM_TRACE("WeakCall object : " + ptr->getFullClassname() );
    }

    ~WeakCall()
    {
    }

    //------------------------------------------------------------------------------

    R operator ()() const
    {
        std::shared_ptr< T const > ptr(this->m_weakPtr.lock());

        if (!ptr)
        {
            m_worker.reset();
            // will throw an exception because m_weakPtr is expired
            std::shared_ptr< T const > ptr(this->m_weakPtr);
        }

        ::fwCore::mt::ReadLock lock(ptr->m_workerMutex);

        std::shared_ptr< ::fwThread::Worker > worker = m_worker.lock();

        if (worker && ptr->m_worker != worker)
        {
            //Worker changed since WeakCall creation
            FW_RAISE_EXCEPTION(::fwCom::exception::WorkerChanged("Worker changed since WeakCall creation"));
        }

        this->m_weakPtr.reset();

        return this->m_func();
    }

    protected:
        mutable std::weak_ptr< T const > m_weakPtr;
        std::function< R() > m_func;
        mutable std::weak_ptr< ::fwThread::Worker > m_worker;
};

/// Returns weak call from given object and function.
template <typename T, typename R >
WeakCall<T, R> weakcall( const std::shared_ptr< T const >& ptr, std::function< R() > f)
{
    return WeakCall<T, R>(ptr, f);
}

/// Returns weak call from given object, function and mutex.
template <typename T, typename R >
WeakCall<T, R> weakcall( const std::shared_ptr< T const >& ptr, std::function< R() > f,
                         const std::shared_ptr< ::fwThread::Worker >& m)
{
    return WeakCall<T, R>(ptr, f, m);
}

} // namespace util

} // namespace fwCom

#endif /* __FWCOM_UTIL_WEAKCALL_HPP__ */

