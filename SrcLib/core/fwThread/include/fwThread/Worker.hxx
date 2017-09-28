/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
