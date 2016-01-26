/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_TASKHANDLER_HXX__
#define __FWTHREAD_TASKHANDLER_HXX__

namespace fwThread
{
template <typename R>
TaskHandler<R>::TaskHandler(::boost::packaged_task<R>& task) : m_task(::boost::move(task))
{
}

template <typename R>
TaskHandler<R>::TaskHandler(const TaskHandler& that) : m_task(::boost::move(that.m_task))
{
}

template <typename R>
void TaskHandler<R>::operator ()() const
{
    this->m_task();
}


template <typename R>
inline ::boost::function< void() > moveTaskIntoFunction(::boost::packaged_task<R>& task)
{
    return TaskHandler<R>(task);
}

} //namespace fwThread

#endif //__FWTHREAD_TASKHANDLER_HXX__
