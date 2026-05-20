/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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
 * @brief Inverts a boolean value from a string_serializable source.
 *
 * This service interprets the source as a boolean and assigns the inverse value to the target.
 * If the source cannot be converted to a boolean, an exception is thrown.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::data::inverse">
           <in key="source" uid="..." />
           <inout key="target" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b source [sight::data::string_serializable]: define the source object to inverse.
 *
 * @subsection In-Out In-Out
 * - \b target [sight::data::string_serializable]: define the target object to update.
 */
class inverse final : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(inverse, service::filter);

    /// Creates the service.
    inverse();

    /// Destroys the service.
    ~inverse() final = default;

protected:

    //// Calls update when the source changes
    service::connections_t auto_connections() const final;

    //// Configures the service (does nothing)
    void configuring(const config_t& _config) final;

    /// Does nothing
    void starting() final;

    /// Inverts the boolean value from source and updates target
    /// @throw if the source cannot be converted to a boolean
    void updating() final;

    /// Does nothing
    void stopping() final;

private:

    sight::data::ptr<sight::data::string_serializable, sight::data::access::in> m_source {this, "source"};
    sight::data::ptr<sight::data::string_serializable, sight::data::access::inout> m_target {this, "target"};
};

} // namespace sight::module::data.
