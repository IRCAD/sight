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

#ifndef __FWTHREAD_TASKHANDLER_HXX__
#define __FWTHREAD_TASKHANDLER_HXX__

namespace fwThread
{
template <typename R>
TaskHandler<R>::TaskHandler(std::packaged_task<R>& task) :
    m_task(std::move(task))
{
}

template <typename R>
TaskHandler<R>::TaskHandler(const TaskHandler& that) :
    m_task(std::move(that.m_task))
{
}

//------------------------------------------------------------------------------

template <typename R>
void TaskHandler<R>::operator ()() const
{
    this->m_task();
}

//------------------------------------------------------------------------------

template <typename R>
inline std::function< void() > moveTaskIntoFunction(std::packaged_task<R>& task)
{
    return TaskHandler<R>(task);
}

} //namespace fwThread

#endif //__FWTHREAD_TASKHANDLER_HXX__
