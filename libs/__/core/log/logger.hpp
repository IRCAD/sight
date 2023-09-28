/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "core/log/log.hpp"

#include <core/tools/object.hpp>

#include <string>

namespace sight::core::log
{

/**
 * @brief Logger class used to store logs
 */
class CORE_CLASS_API logger : public core::tools::object
{
public:

    SIGHT_DECLARE_CLASS(logger, core::tools::object);

    typedef std::vector<core::log::log> log_container_type;
    typedef log_container_type::iterator iterator_type;
    typedef log_container_type::const_iterator const_iterator_type;
    typedef log_container_type::reverse_iterator reverse_iterator_type;
    typedef log_container_type::const_reverse_iterator const_reverse_iterator_type;

    /// Constructor
    CORE_API logger();

    /// Destructor
    CORE_API ~logger() override;

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
    CORE_API core::log::log get_log(unsigned int index);

    /// Returns the number of logs
    CORE_API std::size_t count() const;

    /**
     * @brief Return the number of logs matching the specified level
     * @param[in] level Log level
     */
    CORE_API std::size_t count(core::log::log::level_type level) const;

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
        return m_log_container.empty();
    }

    //------------------------------------------------------------------------------

    iterator_type begin()
    {
        return m_log_container.begin();
    }

    //------------------------------------------------------------------------------

    iterator_type end()
    {
        return m_log_container.end();
    }

    //------------------------------------------------------------------------------

    const_iterator_type begin() const
    {
        return m_log_container.begin();
    }

    //------------------------------------------------------------------------------

    const_iterator_type end() const
    {
        return m_log_container.end();
    }

    //------------------------------------------------------------------------------

    reverse_iterator_type rbegin()
    {
        return m_log_container.rbegin();
    }

    //------------------------------------------------------------------------------

    reverse_iterator_type rend()
    {
        return m_log_container.rend();
    }

    //------------------------------------------------------------------------------

    const_reverse_iterator_type rbegin() const
    {
        return m_log_container.rbegin();
    }

    //------------------------------------------------------------------------------

    const_reverse_iterator_type rend() const
    {
        return m_log_container.rend();
    }

protected:

    /**
     * @brief Function used to sort logs
     * @param logA First log
     * @param logB Second log
     */
    static bool log_sorter(const core::log::log& log_a, const core::log::log& log_b);

    /// Log container
    log_container_type m_log_container;
}; // class logger

} // namespace sight::core::log
