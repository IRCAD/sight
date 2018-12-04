/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#ifndef __FWLOG_LOGGER_HPP__
#define __FWLOG_LOGGER_HPP__

#include "fwLog/Log.hpp"
#include "fwLog/config.hpp"

#include <fwTools/Object.hpp>

#include <string>

namespace fwLog
{

/**
 * @brief Logger class used to store logs
 */
class FWLOG_CLASS_API Logger : public ::fwTools::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((Logger)(::fwTools::Object), (()), new Logger);

    typedef std::vector< ::fwLog::Log > LogContainerType;
    typedef LogContainerType::iterator IteratorType;
    typedef LogContainerType::const_iterator ConstIteratorType;
    typedef LogContainerType::reverse_iterator ReverseIteratorType;
    typedef LogContainerType::const_reverse_iterator ConstReverseIteratorType;

    /// Constructor
    FWLOG_API Logger();

    /// Destructor
    FWLOG_API ~Logger();

    /**
     * @brief Add information log into the logger
     * @param[in] message Log message
     */
    FWLOG_API void information(const std::string& message);

    /**
     * @brief Add warning log into the logger
     * @param[in] message Log message
     */
    FWLOG_API void warning(const std::string& message);

    /**
     * @brief Add critical log into the logger
     * @param[in] message Log message
     */
    FWLOG_API void critical(const std::string& message);

    /**
     * @brief Return the log matching the specified index
     * @param[in] index Log's index
     */
    FWLOG_API ::fwLog::Log getLog(unsigned int index);

    /// Returns the number of logs
    FWLOG_API std::size_t count() const;

    /**
     * @brief Return the number of logs matching the specified level
     * @param[in] level Log level
     */
    FWLOG_API std::size_t count(::fwLog::Log::LevelType level) const;

    /**
     * @brief Sort logs according to log levels
     */
    FWLOG_API void sort();

    /**
     * @brief Clear logs
     */
    FWLOG_API void clear();

    /// Return whether the logger contains logs or not
    bool empty() const
    {
        return m_logContainer.empty();
    }

    IteratorType begin()
    {
        return m_logContainer.begin();
    }
    IteratorType end()
    {
        return m_logContainer.end();
    }
    ConstIteratorType begin() const
    {
        return m_logContainer.begin();
    }
    ConstIteratorType end()   const
    {
        return m_logContainer.end();
    }

    ReverseIteratorType rbegin()
    {
        return m_logContainer.rbegin();
    }
    ReverseIteratorType rend()
    {
        return m_logContainer.rend();
    }
    ConstReverseIteratorType rbegin() const
    {
        return m_logContainer.rbegin();
    }
    ConstReverseIteratorType rend()   const
    {
        return m_logContainer.rend();
    }

protected:

    /**
     * @brief Function used to sort logs
     * @param logA First log
     * @param logB Second log
     */
    static bool logSorter(const ::fwLog::Log& logA, const ::fwLog::Log& logB);

    /// Log container
    LogContainerType m_logContainer;


}; // class Logger

} // namespace fwLog

#endif // __FWLOG_LOGGER_HPP__
