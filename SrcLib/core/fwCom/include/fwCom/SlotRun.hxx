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

#include <fwThread/TaskHandler.hpp>

#include "fwCom/exception/NoWorker.hpp"

#include "fwCom/util/WeakCall.hpp"

#include "fwCom/SlotBase.hxx"

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
    ::boost::packaged_task<void>   task(::fwCom::util::weakcall( this->shared_from_this(), this->bindRun( args1, args2, args3 ) ));
    ::boost::unique_future< void > ufuture = task.get_future();
    ::boost::function< void () >   f = ::fwThread::moveTaskIntoFunction(task);
    worker->post(f);

    return ::boost::move(ufuture);
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
    ::boost::packaged_task<void>   task(::fwCom::util::weakcall( this->shared_from_this(), this->bindRun( args1, args2 ) ));
    ::boost::unique_future< void > ufuture = task.get_future();
    ::boost::function< void () >   f = ::fwThread::moveTaskIntoFunction(task);
    worker->post(f);

    return ::boost::move(ufuture);
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
    ::boost::packaged_task<void>   task(::fwCom::util::weakcall( this->shared_from_this(), this->bindRun( args1 ) ));
    ::boost::unique_future< void > ufuture = task.get_future();
    ::boost::function< void () >   f = ::fwThread::moveTaskIntoFunction(task);
    worker->post(f);

    return ::boost::move(ufuture);
}



inline
SlotBase::VoidSharedFutureType
SlotRun< void () >::asyncRun( const ::fwThread::Worker::sptr &worker) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }
    ::boost::packaged_task<void>   task(::fwCom::util::weakcall( this->shared_from_this(), this->bindRun() ));
    ::boost::unique_future< void > ufuture = task.get_future();
    ::boost::function< void () >   f = ::fwThread::moveTaskIntoFunction(task);
    worker->post(f);

    return ::boost::move(ufuture);
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
    ::boost::packaged_task<void>   task(::fwCom::util::weakcall( this->shared_from_this(), this->bindRun( args... ) ));
    ::boost::unique_future< void > ufuture = task.get_future();
    ::boost::function< void () >   f = ::fwThread::moveTaskIntoFunction(task);
    worker->post(f);

    return ::boost::move(ufuture);
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1, typename A2, typename A3 >
inline SlotBase::VoidSharedFutureType SlotRun< void ( A1, A2, A3 ) >::asyncRun( A1 args1, A2 args2, A3 args3 ) const
{
    SlotBase::VoidSharedFutureType ufuture;
    ufuture = this->asyncRun(m_worker, args1, args2, args3 );
    return ufuture;

}



template< typename A1, typename A2 >
inline SlotBase::VoidSharedFutureType SlotRun< void ( A1, A2 ) >::asyncRun( A1 args1, A2 args2 ) const
{
    SlotBase::VoidSharedFutureType ufuture;
    ufuture = this->asyncRun(m_worker, args1, args2 );
    return ufuture;

}



template< typename A1 >
inline SlotBase::VoidSharedFutureType SlotRun< void ( A1 ) >::asyncRun( A1 args1 ) const
{
    SlotBase::VoidSharedFutureType ufuture;
    ufuture = this->asyncRun(m_worker, args1 );
    return ufuture;

}




inline SlotBase::VoidSharedFutureType SlotRun< void () >::asyncRun() const
{
    SlotBase::VoidSharedFutureType ufuture;
    ufuture = this->asyncRun(m_worker);
    return ufuture;

}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A...) >::asyncRun(A... args) const
{
    SlotBase::VoidSharedFutureType ufuture;
    ufuture = this->asyncRun(m_worker, args...);
    return ufuture;

}



#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HXX__ */




