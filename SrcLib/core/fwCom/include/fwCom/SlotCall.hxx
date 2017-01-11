/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCALL_HXX__
#define __FWCOM_SLOTCALL_HXX__

#ifndef __FWCOM_SLOTCALL_HPP__
#error fwCom/SlotCall.hpp not included
#endif

#include "fwCom/exception/NoWorker.hpp"
#include "fwCom/util/WeakCall.hpp"
#include "fwCom/SlotRun.hxx"

#include <fwCore/mt/types.hpp>

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include <boost/thread/future.hpp>

namespace fwCom
{


template< typename R, typename ... A >
::boost::function< R() > SlotCall< R (A ...) >::bindCall( A ... args  ) const
{
    return ::boost::bind( ( R (SlotCall< R (A ...) >::*)( A ... ) const ) &SlotCall< R (A ...) >::call, this,
                          args ... );
}

//-----------------------------------------------------------------------------

template< typename R, typename ... A >
typename SlotCall< R (A ...) >::SharedFutureType SlotCall< R (A ...) >::asyncCall(
    const ::fwThread::Worker::sptr& worker, A ... args ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    return postWeakCall(
        worker,
        ::fwCom::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindCall( args ... )
            )
        );
}

//-----------------------------------------------------------------------------

template< typename R, typename ... A >
typename SlotCall< R (A ...) >::SharedFutureType SlotCall< R (A ...) >::asyncCall(A ... args) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    return postWeakCall(
        this->m_worker,
        ::fwCom::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindCall( args ... ),
            this->m_worker
            )
        );
}

//-----------------------------------------------------------------------------

template< typename R, typename ... A >
template< typename WEAKCALL >
::boost::shared_future< R > SlotCall< R (A ...) >::postWeakCall( const ::fwThread::Worker::sptr& worker, WEAKCALL f )
{
    ::boost::packaged_task< R > task( f );
    ::boost::future< R > ufuture = task.get_future();

    ::boost::function< void () > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ::boost::move(ufuture);
}


} // namespace fwCom

#endif /* __FWCOM_SLOTCALL_HXX__ */

