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

/**
 * @class   TaskHandler.
 * @brief   This class encapsulates packaged task and provides a copy constructor.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
template <typename R>
struct TaskHandler
{
    /// Constructor, moves task.
    TaskHandler(::boost::packaged_task<R>& task);

    /// Copy constructor, moves task.
    TaskHandler(const TaskHandler& that);

    /// Executes intern packaged task.
    void operator ()() const;

protected:

    /// Copy constructor forbidden.
    TaskHandler& operator=( const TaskHandler& ){};

private:
    /// Encapsulated packaged task.
    mutable ::boost::packaged_task<R> m_task;
};

/// Encapsulate a packaged_task into a boost function to be easily posted on a worker.
template <typename R>
inline ::boost::function< void() > moveTaskIntoFunction(::boost::packaged_task<R>& task);

} //namespace fwThread

#include <fwThread/TaskHandler.hxx>

#endif //__FWTHREAD_TASKHANDLER_HPP__
