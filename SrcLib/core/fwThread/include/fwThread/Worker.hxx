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

#ifndef __FWTHREAD_WORKER_HXX__
#define __FWTHREAD_WORKER_HXX__

#include <fwThread/TaskHandler.hpp>

#include <future>

namespace fwThread
{

//------------------------------------------------------------------------------

template< typename R, typename TASK >
std::shared_future< R > Worker::postTask(TASK f)
{
    std::packaged_task< R() > task( f );
    std::future< R > ufuture = task.get_future();

    std::function< void() > ftask = ::fwThread::moveTaskIntoFunction(task);

    if( ::fwThread::getCurrentThreadId() == this->getThreadId())
    {
        ftask();
    }
    else
    {
        this->post(ftask);
    }

    return std::move(ufuture);
}

} //namespace fwThread

#endif // __FWTHREAD_WORKER_HXX__
