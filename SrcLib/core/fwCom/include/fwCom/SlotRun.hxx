/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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
#ifndef __FWCOM_SLOTRUN_HXX__
#define __FWCOM_SLOTRUN_HXX__

#ifndef __FWCOM_SLOTRUN_HPP__
#error fwCom/SlotRun.hpp not included
#endif

#include "fwCom/exception/NoWorker.hpp"
#include "fwCom/SlotBase.hxx"
#include "fwCom/util/WeakCall.hpp"

#include <fwCore/mt/types.hpp>

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include <future>

namespace fwCom
{

//------------------------------------------------------------------------------

template< typename ... A >
inline std::function< void() > SlotRun< void (A ...) >::bindRun( A ... args  ) const
{
    return std::bind( ( void (SelfType::*)( A ... ) const ) &SelfType::run, this, args ... );
}

//-----------------------------------------------------------------------------

template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A ...) >::asyncRun(
    const ::fwThread::Worker::sptr& worker, A ... args ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    return postWeakCall< void >(
        worker,
        ::fwCom::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindRun( args ... )
            )
        );
}

//-----------------------------------------------------------------------------

template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A ...) >::asyncRun(A ... args) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    return postWeakCall< void >(
        m_worker,
        ::fwCom::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindRun( args ... ),
            this->m_worker
            )
        );
}

//-----------------------------------------------------------------------------

// Copied from fwThread::Worker because of issues with gcc 4.2 and template
// keyword
template< typename ... A >
template< typename R, typename WEAKCALL >
std::shared_future< R > SlotRun< void (A ...) >::postWeakCall( const ::fwThread::Worker::sptr& worker, WEAKCALL f )
{
    std::packaged_task< R() > task( f );
    std::future< R > ufuture = task.get_future();

    std::function< void() > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return std::move(ufuture);
}

//-----------------------------------------------------------------------------

} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HXX__ */

