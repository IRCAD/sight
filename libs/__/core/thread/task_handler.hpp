/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include <future>

namespace sight::core::thread
{

/**
 * @brief   This class encapsulates packaged task and provides a copy constructor.
 */
template<typename R>
struct task_handler
{
    /// Constructor, moves task.
    task_handler(std::packaged_task<R>& task);

    /// Copy constructor, moves task.
    task_handler(const task_handler& that);

    /// Copy constructor forbidden.
    task_handler& operator=(const task_handler&) = delete;

    /// Executes intern packaged task.
    void operator()() const;

    private:

        /// Encapsulated packaged task.
        mutable std::packaged_task<R> m_task;
};

/// Encapsulate a packaged_task into a boost function to be easily posted on a worker.
template<typename R>
inline std::function<void()> move_task_into_function(std::packaged_task<R>& task);

} //namespace sight::core::thread

#include <core/thread/task_handler.hxx>
