/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/log/log.hpp"

#include <core/tools/object.hpp>

#include <string>

namespace sight::core::log
{

/**
 * @brief Logger class used to store logs
 */
class SIGHT_CORE_CLASS_API logger : public core::tools::object
{
public:

    SIGHT_DECLARE_CLASS(logger, core::tools::object);

    using log_container_t             = std::vector<core::log::log>;
    using iterator_type               = log_container_t::iterator;
    using const_iterator_type         = log_container_t::const_iterator;
    using reverse_iterator_type       = log_container_t::reverse_iterator;
    using const_reverse_iterator_type = log_container_t::const_reverse_iterator;

    /// Constructor
    SIGHT_CORE_API logger();

    /// Destructor
    SIGHT_CORE_API ~logger() override;

    /**
     * @brief Add information log into the logger
     * @param[in] _message Log message
     */
    SIGHT_CORE_API void information(const std::string& _message);

    /**
     * @brief Add warning log into the logger
     * @param[in] _message Log message
     */
    SIGHT_CORE_API void warning(const std::string& _message);

    /**
     * @brief Add critical log into the logger
     * @param[in] _message Log message
     */
    SIGHT_CORE_API void critical(const std::string& _message);

    /**
     * @brief Return the log matching the specified index
     * @param[in] _index Log's index
     */
    SIGHT_CORE_API core::log::log get_log(unsigned int _index);

    /// Returns the number of logs
    SIGHT_CORE_API std::size_t count() const;

    /**
     * @brief Return the number of logs matching the specified level
     * @param[in] _level Log level
     */
    SIGHT_CORE_API std::size_t count(core::log::log::level_t _level) const;

    /**
     * @brief Sort logs according to log levels
     */
    SIGHT_CORE_API void sort();

    /**
     * @brief Clear logs
     */
    SIGHT_CORE_API void clear();

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
     * @param _log_a First log
     * @param _log_b Second log
     */
    static bool log_sorter(const core::log::log& _log_a, const core::log::log& _log_b);

    /// Log container
    log_container_t m_log_container;
}; // class logger

} // namespace sight::core::log
