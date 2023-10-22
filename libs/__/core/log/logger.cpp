/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "core/log/logger.hpp"

namespace sight::core::log
{

logger::logger()
= default;

//-----------------------------------------------------------------------------

logger::~logger()
= default;

//-----------------------------------------------------------------------------

void logger::information(const std::string& _message)
{
    core::log::log log(core::log::log::INFORMATION, _message);
    m_log_container.push_back(log);
    SIGHT_INFO(_message);
}

//-----------------------------------------------------------------------------

void logger::warning(const std::string& _message)
{
    core::log::log log(core::log::log::WARNING, _message);
    m_log_container.push_back(log);
    SIGHT_WARN(_message);
}

//-----------------------------------------------------------------------------

void logger::critical(const std::string& _message)
{
    core::log::log log(core::log::log::CRITICAL, _message);
    m_log_container.push_back(log);
    SIGHT_ERROR(_message);
}

//-----------------------------------------------------------------------------

core::log::log logger::get_log(unsigned int _index)
{
    SIGHT_ASSERT("Please be sure to provide an index lower than the number of items.", _index < this->count());
    return m_log_container[_index];
}

//-----------------------------------------------------------------------------

std::size_t logger::count() const
{
    return m_log_container.size();
}

//-----------------------------------------------------------------------------

std::size_t logger::count(core::log::log::level_type _level) const
{
    std::size_t count = 0;
    for(const core::log::log& log : m_log_container)
    {
        if(log.get_level() == _level)
        {
            ++count;
        }
    }

    return count;
}

//-----------------------------------------------------------------------------

void logger::sort()
{
    std::sort(m_log_container.begin(), m_log_container.end(), logger::log_sorter);
}

//-----------------------------------------------------------------------------

void logger::clear()
{
    m_log_container.clear();
}

//-----------------------------------------------------------------------------

bool logger::log_sorter(const core::log::log& _log_a, const core::log::log& _log_b)
{
    return _log_a.get_level() > _log_b.get_level();
}

//-----------------------------------------------------------------------------

} //namespace sight::core::log
