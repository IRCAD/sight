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

#include "core/log/Logger.hpp"

namespace sight::core::log
{

Logger::Logger()
= default;

//-----------------------------------------------------------------------------

Logger::~Logger()
= default;

//-----------------------------------------------------------------------------

void Logger::information(const std::string& message)
{
    core::log::Log log(core::log::Log::INFORMATION, message);
    m_logContainer.push_back(log);
    SIGHT_INFO(message);
}

//-----------------------------------------------------------------------------

void Logger::warning(const std::string& message)
{
    core::log::Log log(core::log::Log::WARNING, message);
    m_logContainer.push_back(log);
    SIGHT_WARN(message);
}

//-----------------------------------------------------------------------------

void Logger::critical(const std::string& message)
{
    core::log::Log log(core::log::Log::CRITICAL, message);
    m_logContainer.push_back(log);
    SIGHT_ERROR(message);
}

//-----------------------------------------------------------------------------

core::log::Log Logger::getLog(unsigned int index)
{
    SIGHT_ASSERT("Please be sure to provide an index lower than the number of items.", index < this->count());
    return m_logContainer[index];
}

//-----------------------------------------------------------------------------

std::size_t Logger::count() const
{
    return m_logContainer.size();
}

//-----------------------------------------------------------------------------

std::size_t Logger::count(core::log::Log::LevelType level) const
{
    std::size_t count = 0;
    for(const core::log::Log& log : m_logContainer)
    {
        if(log.getLevel() == level)
        {
            ++count;
        }
    }

    return count;
}

//-----------------------------------------------------------------------------

void Logger::sort()
{
    std::sort(m_logContainer.begin(), m_logContainer.end(), Logger::logSorter);
}

//-----------------------------------------------------------------------------

void Logger::clear()
{
    m_logContainer.clear();
}

//-----------------------------------------------------------------------------

bool Logger::logSorter(const core::log::Log& logA, const core::log::Log& logB)
{
    return logA.getLevel() > logB.getLevel();
}

//-----------------------------------------------------------------------------

} //namespace sight::core::log
