/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <boost/noncopyable.hpp>

#include <map>
#include <string>
#include <vector>

namespace sight::core::runtime::detail::profile
{

/**
 * @brief   Activates a given module with optional parameters.
 */
class activater : public boost::noncopyable
{
public:

    /**
     * @brief       Constructor
     *
     * @param[in]   identifier  a string containing a module identifier
     * @param[in]   version     a string a module version
     */
    activater(std::string identifier, const std::string& version);

    /**
     * @brief       Adds a new parameter to the activater.
     *
     * @param[in]   identifier  a string containing the parameter name
     * @param[in]   value       a string containing the parameter value
     */
    void add_parameter(const std::string& identifier, const std::string& value);

    /**
     * @brief       Adds a new disable extension point  to the activater.
     *
     * @param[in]   identifier    a string containing the parameter name
     */
    void add_disable_extension_point(const std::string& identifier);

    /**
     * @brief       Adds a new disable extension  to the activater.
     *
     * @param[in]   identifier    a string containing the parameter name
     */
    void add_disable_extension(const std::string& identifier);

    /**
     * @brief   Applies the activater on the system.
     *
     * @remark  This method should be called directly.
     */
    void apply();

private:

    typedef std::map<std::string, std::string> parameter_container;
    typedef std::vector<std::string> disable_extension_point_container;
    typedef std::vector<std::string> disable_extension_container;

    const std::string m_identifier; ///< a module identifier

    parameter_container m_parameters;                             ///< all parameters
    disable_extension_point_container m_disable_extension_points; ///< all disable extension points
    disable_extension_container m_disable_extensions;             ///< all disable extensions
};

} // namespace sight::core::runtime::detail::profile
