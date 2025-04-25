/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <data/string_serializable.hpp>

#include <service/filter.hpp>

namespace sight::module::data
{

/**
 * @brief This service associates an input property to another property.
 *
 * This allows mapping any string_serializable data to another string_serializable data, like of course key strings
 * to key string values, but also colours to strings, booleans to strings, etc...
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::data::translate" >
           <map>
              <entry key="green" value="#00FF00" />
              <entry key="red" value="#FF0000" />
           </map>
           <in key="key" uid="..." />
           <inout key="value" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b key [sight::data::string_serializable]: define the source object to translate.
 *
 * @subsection In-Out In-Out
 * - \b value [sight::data::string_serializable]: define the output object.
 *
 * @subsection Configuration Configuration
 * - \b map: List of key/values pairs.
 */
class translate final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(translate, service::filter);

    /// Creates the service.
    translate();

    /// Destroys the service.
    ~translate() final = default;

protected:

    //// Calls update when the key changes
    service::connections_t auto_connections() const override;

    //// Configures the service
    void configuring(const config_t& _config) final;

    /// Translates if the key is set
    void starting() final;

    /// Translates
    /// @throw if the key is not set
    void updating() final;

    /// Sets the output to null.
    void stopping() final;

private:

    std::map<std::string, std::string> m_map;

    sight::data::ptr<sight::data::string_serializable, sight::data::access::in> m_key {this, "key"};
    sight::data::ptr<sight::data::string_serializable, sight::data::access::inout> m_value {this, "value"};
};

} // namespace sight::module::data.
