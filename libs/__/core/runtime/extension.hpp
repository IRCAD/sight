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
#include "core/runtime/module_element.hpp"
#include "core/runtime/types.hpp"

#include <boost/noncopyable.hpp>

#include <string>

namespace sight::core::runtime
{

class module;

/**
 * @brief   Defines the extension class.
 */
class CORE_CLASS_API extension : public module_element,
                                 public boost::noncopyable
{
public:

    /**
     * @brief       Constructor.
     *
     * @param[in]   module  a pointer to the module the extension is attached to
     * @param[in]   id      a string containing the extension identifier
     * @param[in]   point   a string containing the extension point identifier
     *
     * @todo        test parameters validity
     */
    extension(
        std::shared_ptr<module> _module,
        const std::string& _id,
        const std::string& _point
    );

    /**
     * @brief   Retrieves the extension identifier.
     *
     * @return  a string containing the extension identifier (may be empty)
     */
    [[nodiscard]] CORE_API const std::string& identifier() const;

    /**
     * @brief   Retrieves the extension point identifier.
     *
     * @return  a string containing the extension point identifier
     */
    [[nodiscard]] CORE_API const std::string& point() const;

    /**
     * @brief   Retrieves the extension point configuration.
     *
     * @return  a string containing the extension point configuration
     */
    [[nodiscard]] CORE_API virtual const config_t& get_config() const = 0;

private:

    /// A string containing the extension identifier
    const std::string M_ID;
    /// A string containing the extension point identifier the extension will be connected to
    const std::string M_POINT;
    /// The configuration of the extension
    config_t m_config;
};

} // namespace sight::core::runtime
