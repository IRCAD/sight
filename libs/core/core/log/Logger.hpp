/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/log/Log.hpp"

#include <core/tools/Object.hpp>

#include <string>

namespace sight::core::log
{

/**
 * @brief Logger class used to store logs
 */
class CORE_CLASS_API Logger : public core::tools::Object
{
public:

    SIGHT_DECLARE_CLASS(Logger, core::tools::Object, new Logger);

    typedef std::vector<core::log::Log> LogContainerType;
    typedef LogContainerType::iterator IteratorType;
    typedef LogContainerType::const_iterator ConstIteratorType;
    typedef LogContainerType::reverse_iterator ReverseIteratorType;
    typedef LogContainerType::const_reverse_iterator ConstReverseIteratorType;

    /// Constructor
    CORE_API Logger();

    /// Destructor
    CORE_API ~Logger() override;

    /**
     * @brief Add information log into the logger
     * @param[in] message Log message
     */
    CORE_API void information(const std::string& message);

    /**
     * @brief Add warning log into the logger
     * @param[in] message Log message
     */
    CORE_API void warning(const std::string& message);

    /**
     * @brief Add critical log into the logger
     * @param[in] message Log message
     */
    CORE_API void critical(const std::string& message);

    /**
     * @brief Return the log matching the specified index
     * @param[in] index Log's index
     */
    CORE_API core::log::Log getLog(unsigned int index);

    /// Returns the number of logs
    CORE_API std::size_t count() const;

    /**
     * @brief Return the number of logs matching the specified level
     * @param[in] level Log level
     */
    CORE_API std::size_t count(core::log::Log::LevelType level) const;

    /**
     * @brief Sort logs according to log levels
     */
    CORE_API void sort();

    /**
     * @brief Clear logs
     */
    CORE_API void clear();

    /// Return whether the logger contains logs or not
    bool empty() const
    {
        return m_logContainer.empty();
    }

    //------------------------------------------------------------------------------

    IteratorType begin()
    {
        return m_logContainer.begin();
    }

    //------------------------------------------------------------------------------

    IteratorType end()
    {
        return m_logContainer.end();
    }

    //------------------------------------------------------------------------------

    ConstIteratorType begin() const
    {
        return m_logContainer.begin();
    }

    //------------------------------------------------------------------------------

    ConstIteratorType end() const
    {
        return m_logContainer.end();
    }

    //------------------------------------------------------------------------------

    ReverseIteratorType rbegin()
    {
        return m_logContainer.rbegin();
    }

    //------------------------------------------------------------------------------

    ReverseIteratorType rend()
    {
        return m_logContainer.rend();
    }

    //------------------------------------------------------------------------------

    ConstReverseIteratorType rbegin() const
    {
        return m_logContainer.rbegin();
    }

    //------------------------------------------------------------------------------

    ConstReverseIteratorType rend() const
    {
        return m_logContainer.rend();
    }

protected:

    /**
     * @brief Function used to sort logs
     * @param logA First log
     * @param logB Second log
     */
    static bool logSorter(const core::log::Log& logA, const core::log::Log& logB);

    /// Log container
    LogContainerType m_logContainer;
}; // class Logger

} // namespace sight::core::log
