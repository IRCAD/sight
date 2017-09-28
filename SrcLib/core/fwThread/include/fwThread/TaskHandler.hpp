/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_TASKHANDLER_HPP__
#define __FWTHREAD_TASKHANDLER_HPP__

#include <future>

namespace fwThread
{

/**
 * @brief   This class encapsulates packaged task and provides a copy constructor.
 */
template <typename R>
struct TaskHandler
{
    /// Constructor, moves task.
    TaskHandler(std::packaged_task<R>& task);

    /// Copy constructor, moves task.
    TaskHandler(const TaskHandler& that);

    /// Executes intern packaged task.
    void operator ()() const;

    protected:

        /// Copy constructor forbidden.
        TaskHandler& operator=( const TaskHandler& )
        {
        }

    private:
        /// Encapsulated packaged task.
        mutable std::packaged_task<R> m_task;
};

/// Encapsulate a packaged_task into a boost function to be easily posted on a worker.
template <typename R>
inline std::function< void() > moveTaskIntoFunction(std::packaged_task<R>& task);

} //namespace fwThread

#include <fwThread/TaskHandler.hxx>

#endif //__FWTHREAD_TASKHANDLER_HPP__
