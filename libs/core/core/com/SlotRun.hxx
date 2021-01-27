/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#if !defined(__FWCOM_SLOTRUN_HPP__)
#error core/com/SlotRun.hpp not included
#endif

#include "core/com/exception/NoWorker.hpp"
#include "core/com/SlotBase.hxx"
#include "core/com/util/WeakCall.hpp"

#include <core/mt/types.hpp>
#include <core/thread/TaskHandler.hpp>
#include <core/thread/Worker.hpp>

#include <future>

namespace sight::core::com
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
    const core::thread::Worker::sptr& worker, A ... args ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( core::com::exception::NoWorker("No valid worker.") );
    }

    return postWeakCall< void >(
        worker,
        core::com::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindRun( args ... )
            )
        );
}

//-----------------------------------------------------------------------------

template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A ...) >::asyncRun(A ... args) const
{
    core::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( core::com::exception::NoWorker("Slot has no worker set.") );
    }

    return postWeakCall< void >(
        m_worker,
        core::com::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindRun( args ... ),
            this->m_worker
            )
        );
}

//-----------------------------------------------------------------------------

// Copied from core::thread::Worker because of issues with gcc 4.2 and template
// keyword
template< typename ... A >
template< typename R, typename WEAKCALL >
std::shared_future< R > SlotRun< void (A ...) >::postWeakCall( const core::thread::Worker::sptr& worker, WEAKCALL f )
{
    std::packaged_task< R() > task( f );
    std::future< R > ufuture = task.get_future();

    std::function< void() > ftask = core::thread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return ufuture;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
