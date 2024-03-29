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

namespace sight::core::thread
{

template<typename R>
task_handler<R>::task_handler(std::packaged_task<R>& _task) :
    m_task(std::move(_task))
{
}

template<typename R>
task_handler<R>::task_handler(const task_handler& _that) :
    m_task(std::move(_that.m_task))
{
}

//------------------------------------------------------------------------------

template<typename R>
void task_handler<R>::operator()() const
{
    this->m_task();
}

//------------------------------------------------------------------------------

template<typename R>
inline std::function<void()> move_task_into_function(std::packaged_task<R>& _task)
{
    return task_handler<R>(_task);
}

} //namespace sight::core::thread
