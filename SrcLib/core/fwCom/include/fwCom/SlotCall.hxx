/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCALL_HXX__
#define __FWCOM_SLOTCALL_HXX__

#ifndef __FWCOM_SLOTCALL_HPP__
#error fwCom/SlotCall.hpp not included
#endif

#include <boost/thread/future.hpp>

#include <fwCore/mt/types.hpp>

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include "fwCom/exception/NoWorker.hpp"

#include "fwCom/util/WeakCall.hpp"

#include "fwCom/SlotRun.hxx"

#include "fwCom/util/log.hpp"

namespace fwCom
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
::boost::function< R() > SlotCall< R ( A1, A2, A3, A4, A5 ) >::bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 ) const
{
    return ::boost::bind( ( R (SlotCall< R ( A1, A2, A3, A4, A5 ) >::*)( A1, A2, A3, A4, A5 )const ) &SlotCall< R ( A1, A2, A3, A4, A5 ) >::call, this, args1, args2, args3, args4, args5 );
}


template< typename R, typename A1, typename A2, typename A3, typename A4 >
::boost::function< R() > SlotCall< R ( A1, A2, A3, A4 ) >::bindCall( A1 args1, A2 args2, A3 args3, A4 args4 ) const
{
    return ::boost::bind( ( R (SlotCall< R ( A1, A2, A3, A4 ) >::*)( A1, A2, A3, A4 )const ) &SlotCall< R ( A1, A2, A3, A4 ) >::call, this, args1, args2, args3, args4 );
}


template< typename R, typename A1, typename A2, typename A3 >
::boost::function< R() > SlotCall< R ( A1, A2, A3 ) >::bindCall( A1 args1, A2 args2, A3 args3 ) const
{
    return ::boost::bind( ( R (SlotCall< R ( A1, A2, A3 ) >::*)( A1, A2, A3 )const ) &SlotCall< R ( A1, A2, A3 ) >::call, this, args1, args2, args3 );
}


template< typename R, typename A1, typename A2 >
::boost::function< R() > SlotCall< R ( A1, A2 ) >::bindCall( A1 args1, A2 args2 ) const
{
    return ::boost::bind( ( R (SlotCall< R ( A1, A2 ) >::*)( A1, A2 )const ) &SlotCall< R ( A1, A2 ) >::call, this, args1, args2 );
}


template< typename R, typename A1 >
::boost::function< R() > SlotCall< R ( A1 ) >::bindCall( A1 args1 ) const
{
    return ::boost::bind( ( R (SlotCall< R ( A1 ) >::*)( A1 )const ) &SlotCall< R ( A1 ) >::call, this, args1 );
}


template< typename R>
::boost::function< R() > SlotCall< R () >::bindCall() const
{
    return ::boost::bind( ( R (SlotCall< R () >::*)()const ) &SlotCall< R () >::call, this);
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ...A >
::boost::function< R() > SlotCall< R (A...) >::bindCall( A...args  ) const
{
    return ::boost::bind( ( R (SlotCall< R (A...) >::*)( A... )const ) &SlotCall< R (A...) >::call, this, args... );
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
    typename SlotCall< R ( A1, A2, A3, A4, A5 ) >::SharedFutureType
    SlotCall< R ( A1, A2, A3, A4, A5 ) >::asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall( args1, args2, args3, args4, args5 ) ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall( args1, args2, args3, args4, args5 ),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2, args3, args4, args5 )
                        )
                );
}



template< typename R, typename A1, typename A2, typename A3, typename A4 >
    typename SlotCall< R ( A1, A2, A3, A4 ) >::SharedFutureType
    SlotCall< R ( A1, A2, A3, A4 ) >::asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2, A3 args3, A4 args4 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall( args1, args2, args3, args4 ) ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall( args1, args2, args3, args4 ),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2, args3, args4 )
                        )
                );
}



template< typename R, typename A1, typename A2, typename A3 >
    typename SlotCall< R ( A1, A2, A3 ) >::SharedFutureType
    SlotCall< R ( A1, A2, A3 ) >::asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2, A3 args3 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall( args1, args2, args3 ) ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall( args1, args2, args3 ),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2, args3 )
                        )
                );
}



template< typename R, typename A1, typename A2 >
    typename SlotCall< R ( A1, A2 ) >::SharedFutureType
    SlotCall< R ( A1, A2 ) >::asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1, A2 args2 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall( args1, args2 ) ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall( args1, args2 ),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2 )
                        )
                );
}



template< typename R, typename A1 >
    typename SlotCall< R ( A1 ) >::SharedFutureType
    SlotCall< R ( A1 ) >::asyncCall( const ::fwThread::Worker::sptr &worker, A1 args1 ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall( args1 ) ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall( args1 ),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1 )
                        )
                );
}



template< typename R>
    typename SlotCall< R () >::SharedFutureType
    SlotCall< R () >::asyncCall( const ::fwThread::Worker::sptr &worker) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall() ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall(),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall()
                        )
                );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ...A >
    typename SlotCall< R (A...) >::SharedFutureType
    SlotCall< R (A...) >::asyncCall( const ::fwThread::Worker::sptr &worker, A... args ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    OSLM_COM("asyncCall '"<< this->getID() <<"' slot");
    // ::boost::packaged_task<R>   task( ::fwCom::util::weakcall( this->shared_from_this(), this->bindCall( args... ) ) );

    // ::boost::packaged_task< R > task(::fwCom::util::weakcall(
    //                             this->shared_from_this(),
    //                             this->bindCall( args... ),
    //                             this->m_workerMutex ));

    // ::boost::future< R > ufuture = task.get_future();
    // ::boost::function< void() > f = ::fwThread::moveTaskIntoFunction(task);
    // worker->post(f);

    // return ::boost::move(ufuture);

    return postWeakCall(
                worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args... )
                        )
                );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
typename SlotCall< R ( A1, A2, A3, A4, A5 ) >::SharedFutureType
SlotCall< R ( A1, A2, A3, A4, A5 ) >::asyncCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R ( A1, A2, A3, A4, A5 ) >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker, args1, args2, args3, args4, args5 );
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2, args3, args4, args5 ),
                        this->m_workerMutex
                        )
                );
}



template< typename R, typename A1, typename A2, typename A3, typename A4 >
typename SlotCall< R ( A1, A2, A3, A4 ) >::SharedFutureType
SlotCall< R ( A1, A2, A3, A4 ) >::asyncCall( A1 args1, A2 args2, A3 args3, A4 args4 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R ( A1, A2, A3, A4 ) >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker, args1, args2, args3, args4 );
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2, args3, args4 ),
                        this->m_workerMutex
                        )
                );
}



template< typename R, typename A1, typename A2, typename A3 >
typename SlotCall< R ( A1, A2, A3 ) >::SharedFutureType
SlotCall< R ( A1, A2, A3 ) >::asyncCall( A1 args1, A2 args2, A3 args3 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R ( A1, A2, A3 ) >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker, args1, args2, args3 );
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2, args3 ),
                        this->m_workerMutex
                        )
                );
}



template< typename R, typename A1, typename A2 >
typename SlotCall< R ( A1, A2 ) >::SharedFutureType
SlotCall< R ( A1, A2 ) >::asyncCall( A1 args1, A2 args2 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R ( A1, A2 ) >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker, args1, args2 );
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1, args2 ),
                        this->m_workerMutex
                        )
                );
}



template< typename R, typename A1 >
typename SlotCall< R ( A1 ) >::SharedFutureType
SlotCall< R ( A1 ) >::asyncCall( A1 args1 ) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R ( A1 ) >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker, args1 );
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args1 ),
                        this->m_workerMutex
                        )
                );
}



template< typename R>
typename SlotCall< R () >::SharedFutureType
SlotCall< R () >::asyncCall() const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R () >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker);
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall(),
                        this->m_workerMutex
                        )
                );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ... A >
typename SlotCall< R (A...) >::SharedFutureType
SlotCall< R (A...) >::asyncCall(A... args) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    // typename SlotCall< R (A...) >::SharedFutureType ufuture;
    // ufuture = this->asyncCall(this->m_worker, args...);
    // return ufuture;

    return postWeakCall(
                this->m_worker,
                ::fwCom::util::weakcall(
                        this->shared_from_this(),
                        this->bindCall( args... ),
                        this->m_workerMutex
                        )
                );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R ( A1, A2, A3, A4, A5 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename R, typename A1, typename A2, typename A3, typename A4 >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R ( A1, A2, A3, A4 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename R, typename A1, typename A2, typename A3 >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R ( A1, A2, A3 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename R, typename A1, typename A2 >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R ( A1, A2 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename R, typename A1 >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R ( A1 ) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



template< typename R>
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R () >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ... A >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R (A...) >::postWeakCall( const ::fwThread::Worker::sptr &worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOTCALL_HXX__ */

