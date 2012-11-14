/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_TASKHANDLER_HPP__
#define __FWTHREAD_TASKHANDLER_HPP__

#include <boost/asio/io_service.hpp>

namespace fwThread
{

template <typename R>
struct TaskHandler
{
    TaskHandler(boost::packaged_task<R>& task) : m_Task(boost::move(task))
    {}

    TaskHandler(const TaskHandler& that) : m_Task(boost::move(that.m_Task))
    {}

    void operator ()()
    {
        this->m_Task();
    }

private:
    mutable boost::packaged_task<R> m_Task;
};


template <typename R>
inline ::boost::function< void() > moveTaskIntoFunction(boost::packaged_task<R>& task)
{
    return TaskHandler<R>(task);
}

} //namespace fwThread

#endif //__FWTHREAD_TASKHANDLER_HPP__
