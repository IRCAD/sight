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

#include <data/string.hpp>
#include <data/string_serializable.hpp>

#include <service/filter.hpp>

namespace sight::module::data
{

/**
 * @brief This service concatenate string_serializable data.
 *
 * The service concatenates all the input fragments into the target object. The concatenation follows the order of the
 * fragments input group. If a format properties is provided, it is used to create the target string.
 * (follows the std::format syntax).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::data::concat" >
        <properties format="Hello {} !" />
        <inout key="target" uid="..." />
        <inout group="fragments">
            <key name="object1" uid="..." />
            <key name="object2" uid="..." />
            ..
        </inout>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b fragments [sight::data::string_serializable]: define the string objects to concat.
 *
 * @subsection In-Out In-Out
 * - \b target [sight::data::string_serializable]: define the target object.
 */
class concat final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(concat, service::filter);

    /// Creates the service.
    concat();

    /// Destroys the service.
    ~concat() final = default;

protected:

    /// Calls update when the fragments change
    service::connections_t auto_connections() const final;

    /// Does nothing
    void configuring(const config_t& _config) final;

    /// Does nothing
    void starting() final;

    /// Concatenates
    void updating() final;

    /// Does nothing
    void stopping() final;

private:

    /// Inputs: fragments to concat
    sight::data::ptr_vector<sight::data::string_serializable, sight::data::access::in> m_fragments {this, "fragments"};

    /// In-Out: target to store the result
    sight::data::ptr<sight::data::string_serializable, sight::data::access::inout> m_target {this, "target"};

    // Properties
    sight::data::property<sight::data::string> m_format {this, "format", std::string()};
};

} // namespace sight::module::data.
