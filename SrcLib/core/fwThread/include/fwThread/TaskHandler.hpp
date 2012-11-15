/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_TASKHANDLER_HPP__
#define __FWTHREAD_TASKHANDLER_HPP__

#include <boost/thread/future.hpp>

namespace fwThread
{

template <typename R>
struct TaskHandler
{
    TaskHandler(::boost::packaged_task<R>& task);

    TaskHandler(const TaskHandler& that);

    void operator ()() const;

protected:

    /// Copy constructor forbidden
    TaskHandler& operator=( const TaskHandler& ){};

private:
    mutable ::boost::packaged_task<R> m_task;
};


template <typename R>
inline ::boost::function< void() > moveTaskIntoFunction(::boost::packaged_task<R>& task);

} //namespace fwThread

#include <fwThread/TaskHandler.hxx>

#endif //__FWTHREAD_TASKHANDLER_HPP__
