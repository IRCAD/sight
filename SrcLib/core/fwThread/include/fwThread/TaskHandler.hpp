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
