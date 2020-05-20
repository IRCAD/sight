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

#if !defined(__FWCOM_SLOTCALL_HPP__)
#error fwCom/SlotCall.hpp not included
#endif

#include "fwCom/exception/NoWorker.hpp"
#include "fwCom/SlotRun.hxx"
#include "fwCom/util/WeakCall.hpp"

#include <fwCore/mt/types.hpp>

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include <future>

namespace fwCom
{

//------------------------------------------------------------------------------

template< typename R, typename ... A >
std::function< R() > SlotCall< R(A ...) >::bindCall( A ... args  ) const
{
    return std::bind( ( R(SlotCall< R(A ...) >::*)( A ... ) const ) &SlotCall< R(A ...) >::call, this,
                      args ... );
}

//-----------------------------------------------------------------------------

template< typename R, typename ... A >
typename SlotCall< R(A ...) >::SharedFutureType SlotCall< R(A ...) >::asyncCall(
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
typename SlotCall< R(A ...) >::SharedFutureType SlotCall< R(A ...) >::asyncCall(A ... args) const
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
std::shared_future< R > SlotCall< R(A ...) >::postWeakCall( const ::fwThread::Worker::sptr& worker, WEAKCALL f )
{
    std::packaged_task< R() > task( f );
    std::future< R > ufuture = task.get_future();

    std::function< void() > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ufuture;
}

} // namespace fwCom
