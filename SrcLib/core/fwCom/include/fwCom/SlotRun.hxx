/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTRUN_HXX__
#define __FWCOM_SLOTRUN_HXX__

#ifndef __FWCOM_SLOTRUN_HPP__
#error fwCom/SlotRun.hpp not included
#endif

#include <boost/thread/future.hpp>

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include <fwCore/mt/types.hpp>

#include "fwCom/exception/NoWorker.hpp"

#include "fwCom/util/WeakCall.hpp"

#include "fwCom/SlotBase.hxx"

#include "fwCom/util/log.hpp"

namespace fwCom
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
inline ::boost::function< void() > SlotRun< void ( A1, A2, A3 ) >::bindRun( A1 args1, A2 args2, A3 args3 ) const
{
    return ::boost::bind( ( void (SelfType::*)( A1, A2, A3 ) const ) &SelfType::run, this, args1, args2, args3 );
}


template< typename A1, typename A2 >
inline ::boost::function< void() > SlotRun< void ( A1, A2 ) >::bindRun( A1 args1, A2 args2 ) const
{
    return ::boost::bind( ( void (SelfType::*)( A1, A2 ) const ) &SelfType::run, this, args1, args2 );
}


template< typename A1 >
inline ::boost::function< void() > SlotRun< void ( A1 ) >::bindRun( A1 args1 ) const
{
    return ::boost::bind( ( void (SelfType::*)( A1 ) const ) &SelfType::run, this, args1 );
}



inline ::boost::function< void() > SlotRun< void () >::bindRun() const
{
    return ::boost::bind( ( void (SelfType::*)() const ) &SelfType::run, this);
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
inline ::boost::function< void() > SlotRun< void (A...) >::bindRun( A...args  ) const
{
    return ::boost::bind( ( void (SelfType::*)( A... ) const ) &SelfType::run, this, args... );
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
inline
SlotBase::VoidSharedFutureType
SlotRun< void ( A1, A2, A3 ) >::asyncRun( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2, A3 args3 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncRun '"<< this->getID() <<"' slot");

    return postWeakCall< void >(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args1, args2, args3 )
                        )
                );
}


template< typename A1, typename A2 >
inline
SlotBase::VoidSharedFutureType
SlotRun< void ( A1, A2 ) >::asyncRun( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncRun '"<< this->getID() <<"' slot");

    return postWeakCall< void >(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args1, args2 )
                        )
                );
}


template< typename A1 >
inline
SlotBase::VoidSharedFutureType
SlotRun< void ( A1 ) >::asyncRun( const ::fwThread::Worker::sptr &worker, A1 args1 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncRun '"<< this->getID() <<"' slot");

    return postWeakCall< void >(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args1 )
                        )
                );
}



inline
SlotBase::VoidSharedFutureType
SlotRun< void () >::asyncRun( const ::fwThread::Worker::sptr &worker) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncRun '"<< this->getID() <<"' slot");

    return postWeakCall< void >(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun()
                        )
                );
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
inline
SlotBase::VoidSharedFutureType
SlotRun< void (A...) >::asyncRun( const ::fwThread::Worker::sptr &worker, A... args ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncRun '"<< this->getID() <<"' slot");

    return postWeakCall< void >(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args... )
                        )
                );
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
inline SlotBase::VoidSharedFutureType SlotRun< void ( A1, A2, A3 ) >::asyncRun( A1 args1, A2 args2, A3 args3 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }


    return postWeakCall< void >(
                m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args1, args2, args3 ),
                        this->m_workerMutex
                        )
                );

}




template< typename A1, typename A2 >
inline SlotBase::VoidSharedFutureType SlotRun< void ( A1, A2 ) >::asyncRun( A1 args1, A2 args2 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }


    return postWeakCall< void >(
                m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args1, args2 ),
                        this->m_workerMutex
                        )
                );

}




template< typename A1 >
inline SlotBase::VoidSharedFutureType SlotRun< void ( A1 ) >::asyncRun( A1 args1 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }


    return postWeakCall< void >(
                m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args1 ),
                        this->m_workerMutex
                        )
                );

}





inline SlotBase::VoidSharedFutureType SlotRun< void () >::asyncRun() const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }


    return postWeakCall< void >(
                m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun(),
                        this->m_workerMutex
                        )
                );

}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A...) >::asyncRun(A... args) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }


    return postWeakCall< void >(
                m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindRun( args... ),
                        this->m_workerMutex
                        )
                );

}




#endif  // BOOST_NO_VARIADIC_TEMPLATES
// Copied from fwThread::Worker because of issues with gcc 4.2 and template
// keyword
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
template< typename R, typename WEAKCALL >
::boost::shared_future< R > SlotRun< void ( A1, A2, A3 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::unique_future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename A1, typename A2 >
template< typename R, typename WEAKCALL >
::boost::shared_future< R > SlotRun< void ( A1, A2 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::unique_future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename A1 >
template< typename R, typename WEAKCALL >
::boost::shared_future< R > SlotRun< void ( A1 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::unique_future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}




template< typename R, typename WEAKCALL >
::boost::shared_future< R > SlotRun< void () >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::unique_future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
template< typename R, typename WEAKCALL >
::boost::shared_future< R > SlotRun< void (A...) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::unique_future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HXX__ */




